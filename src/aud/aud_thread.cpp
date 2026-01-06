#include<core.h>
#include<aud.h>
#include <atomic>   


int aud_thread(){

    pa_sample_spec spec;
    spec.format = PA_SAMPLE_FLOAT32LE;
    spec.rate = 44100;
    spec.channels = 2;


    // This is one long constructor damn

    // nullptr give default values, MY_SOURCE is my usb port
    int error;
    pa_simple* aud_stream = pa_simple_new(
        nullptr,                        // which server to get from
        "really cool thing im making",  // app name
        PA_STREAM_RECORD,               // recored the playing audio
        MY_SOURCE,                      // from this device
        "Audio Capture",                // what i'm doing
        &spec,                          // pointer to the specs above 
        nullptr,                        // channel layout (stereo/mono etc.) this will use default from thhe source
        nullptr,                        // Buffer attributes (unsure, so just default)
        &error                          // the error handler
    );

    if (!aud_stream) {
        std::cerr << "PulseAudio error: "
                << pa_strerror(error) << "\n";
        return 1;
    }

    // buffer size and memory alloc 
    constexpr size_t FRAMES = 512;
    std::vector<float> buffer(FRAMES * spec.channels);
    // The buffer is a circular queue
    // stereo audio will be interleaved just like admge.

    // what this loop does is 
    //    -block the thread
    //    -fill the buffer completely
    //    -find the RMS value
    //    -unblock


    while (true) {
        if (pa_simple_read(aud_stream, buffer.data(),
                        buffer.size() * sizeof(float),
                        &error) < 0) {
            std::cerr << "Read error: "
                    << pa_strerror(error) << "\n";
            break;
        }

        // Compute RMS volume
        float sum = 0.0f;
        for (float s : buffer) {
            sum += s * s;
        }

        float rms = std::sqrt(sum / buffer.size());
        //std::cout << "\rRMS: " << rms << std::flush;
        audio_buffer.store(rms);
    }

    pa_simple_free(aud_stream);
    return 0;
}