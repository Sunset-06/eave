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
        MY_SOURCE,                      // from this monitor
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

        // converting the stereo input to mono input and applying hann window
        for (int i = 0; i < 512; i++) {
            float window = 0.5f * (1.0f - cosf(2.0f * M_PI * i / 511.0f));
            float mono = (buffer[i * 2] + buffer[i * 2 + 1]) / 2.0f;
            in[i].r = mono * window;
            in[i].i = 0.0f;
        }

        kiss_fft(cfg, in, out);

        // first half of the output arr - positive frequencies
        // out[0] is the dc bin - min frequency
        // out[nfft/2] will be the nyquist frequency - max frequency
        // second half of the out array is negative frequecies and is useless
        
        // getting only the first 5 bins for bass

        int totalBins = 256;
        float barValues[16] = {0};
        Frame currFrame;
        for (int i = 0; i < BARS; i++) {
            int binStart = (int)std::pow(totalBins, (float)i / BARS);
            int binEnd = (int)std::pow(totalBins, (float)(i + 1) / BARS);

            if (binEnd <= binStart) binEnd = binStart + 1;
            if (binEnd > totalBins) binEnd = totalBins;

            float sumMag = 0;
            for (int k = binStart; k < binEnd; k++) {
                float mag = std::sqrt(out[k].r * out[k].r + out[k].i * out[k].i);
                sumMag += mag;
            }

            float avgMag = sumMag / (float)(binEnd - binStart);
            float dB = 20.0f * log10f(avgMag + 1e-6f);
            float normalized = (dB + 50.0f) / 50.0f;
            
            float tilt = std::pow(1.7f, (float)i / (BARS * 0.5f));

            currFrame.bars[i] = std::max(0.0f, normalized * tilt);
        }

        //std::cout << "PUSHED---"<<currFrame.bars[0]<<" "<< currFrame.bars[1] << "\n";


        if (!shared_buffer.buf_push(currFrame)) {
            std::cout<<"Dropped a value\n";
        }
    }

    pa_simple_free(aud_stream);
    kiss_fft_free(cfg);
    return 0;
}