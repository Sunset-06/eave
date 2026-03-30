#include "core.h"
#define STB_IMAGE_IMPLEMENTATION
#include "vis.h"
#include "aud.h"

RingBuffer shared_buffer;
extern bool exit_flag;
Mode current_mode = MODE_BARS;

int main(int argc, char* argv[]) {
    std::thread audio_thread(aud_thread);
    std::thread video_thread(vis_thread);
    std::thread infor_thread(inf_thread);
    // exit_flag = true;
    audio_thread.join();
    video_thread.join();
    infor_thread.join();

    return 0;
}