#ifndef CORE_H
#define CORE_H

#include <iostream>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <atomic>
#include <complex>

#define BUFFER_SIZE 512
#define BARS 16

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

extern RingBuffer shared_buffer;
extern bool exit_flag;

extern int vis_thread();
extern int aud_thread();

#endif
