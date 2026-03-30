#ifndef CORE_H
#define CORE_H

#include <iostream>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <atomic>
#include <complex>
#include <thread>
#include <mutex>

#define GREET "\nBye Bye\n"

#define BUFFER_SIZE 512
#define BARS 20

enum Mode { MODE_BARS, MODE_WAVE };

struct Frame {
    float bars[BARS];
};

struct RingBuffer {
    Frame data[BUFFER_SIZE];
    std::atomic<size_t> write_ptr{0};
    std::atomic<size_t> read_ptr{0};

    bool buf_push(const Frame& frame) {
        size_t curr_write = write_ptr.load();
        size_t next_write = (curr_write + 1) % BUFFER_SIZE;

        // skip write if buffer is full
        if (next_write == read_ptr.load()) {
            return false; 
        }

        //else, write
        data[curr_write] = frame;
        write_ptr.store(next_write);
        return true;
    }

    bool buf_pop(Frame& frame) {
        size_t curr_read = read_ptr.load();

        // skip if buffer is empty
        if (curr_read == write_ptr.load()) {
            return false;
        }

        //else, pop
        frame = data[curr_read];
        read_ptr.store((curr_read + 1) % BUFFER_SIZE);
        return true;
    }
};

struct Metadata {
    std::string title = "Unknown";
    std::string artist = "Unknown";
    std::string album = "Unknown";
    std::string path = "";
    std::mutex mtx;
    int w, h;
    unsigned char* pixel_buffer = nullptr;
    bool flag = false;
};

extern Metadata curr_metadata;

extern RingBuffer shared_buffer;
extern bool exit_flag;
extern Mode current_mode;

extern int vis_thread();
extern int aud_thread();
extern int inf_thread();

#endif