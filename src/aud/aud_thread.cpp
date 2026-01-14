#include<core.h>
#include<aud.h>
#include<atomic>   

int aud_thread(){

    pa_sample_spec spec;
    spec.format = PA_SAMPLE_FLOAT32LE;
    spec.rate = 44100;
    spec.channels = 2;



    // Setting things up for lower latency
    // Here, -1 is default, since I only really want to change the fragsize
    pa_buffer_attr buffer_attr;
    buffer_attr.maxlength = (uint32_t) -1;
    buffer_attr.tlength   = (uint32_t) -1;
    buffer_attr.prebuf    = (uint32_t) -1;
    buffer_attr.minreq    = (uint32_t) -1;
    buffer_attr.fragsize = pa_usec_to_bytes(20 * 1000, &spec);
    // This is going to be sent to the pulse audio constructor

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
        &buffer_attr,                   // Buffer attributes (Defined above)
        &error                          // the error handler
    );

    if (!aud_stream) {
        std::cerr << "PulseAudio error: "
                << pa_strerror(error) << "\n";
        return 1;
    }

    // init the fft library and its input and output array
    kiss_fft_cfg cfg = kiss_fft_alloc(512, 0, nullptr, nullptr);
    kiss_fft_cpx in[512];
    kiss_fft_cpx out[512];

    // buffer size and memory alloc 
    constexpr size_t FRAMES = 512;
    std::vector<float> buffer(FRAMES * spec.channels);
    // The buffer is a circular queue

    // what this loop does is 
    //    -block the thread
    //    -fill the buffer completely
    //    -find the RMS value
    //    -unblock


    while (!exit_flag) {
        if (pa_simple_read(aud_stream, buffer.data(),
                        buffer.size() * sizeof(float),
                        &error) < 0) {
            std::cerr << "Read error: "
                    << pa_strerror(error) << "\n";
            break;
        }

        // converting the stereo input to mono input
        for (int i = 0; i < 512; i++) {
            in[i].r = (buffer[i * 2] + buffer[i * 2 + 1]) / 2.0f;;
            in[i].i = 0.0f;
        }

        kiss_fft(cfg, in, out);

        // first half of the output arr - positive frequencies
        // out[0] is the dc bin - min frequency
        // out[nfft/2] will be the nyquist frequency - max frequency
        // second half of the out array is negative frequecies and is useless
        
        // getting only the first 5 bins for bass
        float max_magnitude = 0;
        for (int k = 1; k < 6; k++) {
            max_magnitude += std::sqrt(out[k].r * out[k].r + out[k].i * out[k].i);
            float mag = std::sqrt(out[k].r * out[k].r + out[k].i * out[k].i);
            if (mag > max_magnitude) max_magnitude = mag;
            //max_magnitude += std::sqrt(out[k].r * out[k].r + out[k].i * out[k].i);
        }

        if (!shared_buffer.buf_push(max_magnitude)) {
            std::cout<<"Dropped a value\n";
        }
    }

    pa_simple_free(aud_stream);
    kiss_fft_free(cfg);
    return 0;
}