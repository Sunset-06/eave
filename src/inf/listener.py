import dbus
import threading
import urllib.request
import os
from urllib.parse import unquote
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib

TEMP_ART_PATH = "/tmp/eave_current_art.jpg"

def process_metadata(metadata):
    """
    Processes metadata in a background thread to prevent blocking 
    the DBus signal listener during downloads or slow I/O.
    """
    try:
        # Extract metadata with defaults
        title = str(metadata.get('xesam:title', 'Unknown'))
        # Artists are usually a list in MPRIS
        artist_list = metadata.get('xesam:artist', ['Unknown'])
        artist = ", ".join([str(a) for a in artist_list])
        album = str(metadata.get('xesam:album', 'Unknown'))
        
        url = str(metadata.get('mpris:artUrl', ''))
        path = ""

        if url:
            if url.startswith("file://"):
                path = unquote(url[7:])
            elif url.startswith("http"):
                try:
                    urllib.request.urlretrieve(url, TEMP_ART_PATH)
                    path = TEMP_ART_PATH
                except Exception as e:
                    path = ""
    
        print(f"{title}|{artist}|{album}|{path}", flush=True)

    except Exception as e:
        pass

def on_change(interface, changed_properties, invalidated_properties):
    """Callback for DBus PropertiesChanged signal."""
    if 'Metadata' in changed_properties:
        metadata = changed_properties['Metadata']
        threading.Thread(
            target=process_metadata, 
            args=(metadata,), 
            daemon=True
        ).start()

def main():
    DBusGMainLoop(set_as_default=True)
    
    try:
        bus = dbus.SessionBus()
    except dbus.exceptions.DBusException:
        print("Error: Could not connect to Session Bus.", flush=True)
        return

    bus.add_signal_receiver(
        on_change,
        dbus_interface="org.freedesktop.DBus.Properties",
        signal_name="PropertiesChanged",
        path="/org/mpris/MediaPlayer2"
    )

    bus.add_signal_receiver(
        on_change,
        dbus_interface="org.freedesktop.DBus.Properties",
        signal_name="PropertiesChanged",
        path="/org/mpris/MediaPlayer2/Player"
    )

    loop = GLib.MainLoop()
    try:
        loop.run()
    except KeyboardInterrupt:
        loop.quit()

if __name__ == "__main__":
    main()