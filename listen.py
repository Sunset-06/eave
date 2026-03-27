import dbus
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib
import urllib.request
from urllib.parse import unquote

def on_change(interface, changed_properties, invalidated_properties):
    if 'Metadata' in changed_properties:
        metadata = changed_properties['Metadata']
        
        title = str(metadata.get('xesam:title', 'Unknown'))
        artist = ", ".join(metadata.get('xesam:artist', ['Unknown']))
        album = str(metadata.get('xesam:album', 'Unknown'))
        path = ""

        if 'mpris:artUrl' in metadata:
            url = str(metadata['mpris:artUrl'])
            
            # read path
            path = unquote(url[7:]) if url.startswith("file://") else "/tmp/current_art.jpg"
            
            # web url
            if url.startswith("http"):
                urllib.request.urlretrieve(url, path)
            
            print(f"curr song: {title} | {artist} | {album} | {path}", flush=True)

DBusGMainLoop(set_as_default=True)
bus = dbus.SessionBus()

bus.add_signal_receiver(
    on_change,
    dbus_interface="org.freedesktop.DBus.Properties",
    signal_name="PropertiesChanged",
    path="/org/mpris/MediaPlayer2"
)

print("Listener started. Waiting for OS pings...", flush=True)
loop = GLib.MainLoop()
loop.run()