#include <iostream>
#include <thread>
#include <atomic>
#include <core.h>
#include <vis.h>
#include <aud.h>

RingBuffer shared_buffer;
extern bool exit_flag;

int main(int argc, char* argv[]) {
    std::thread audio_thread(aud_thread);
    vis_thread();

    exit_flag = true;
    audio_thread.join();

    return 0;
}