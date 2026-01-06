#include <iostream>
#include <thread>
#include <atomic>
#include <core.h>
#include <vis.h>
#include <aud.h>

std::atomic<float> audio_buffer(0.0f);
extern bool exit_flag;

int main(int argc, char* argv[]) {
    std::thread audio_thread(aud_thread);
    vis_thread();

    exit_flag = true;
    if (audio_thread.joinable()) {
        audio_thread.join();
    }

    return 0;
}