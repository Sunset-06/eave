#include <iostream>
#include <thread>
#include <atomic>
#include <core.h>
#include <vis.h>
#include <aud.h>

RingBuffer shared_buffer;
extern bool exit_flag;
Mode current_mode = MODE_BARS;

int main(int argc, char* argv[]) {
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-bars") current_mode = MODE_BARS;
        if (arg == "-wave") current_mode = MODE_WAVE;
    }
    std::thread audio_thread(aud_thread);
    std::thread video_thread(vis_thread);
    // exit_flag = true;
    audio_thread.join();
    video_thread.join();

    return 0;
}