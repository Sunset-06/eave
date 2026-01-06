#ifndef CORE_H
#define CORE_H

#include <iostream>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <atomic>

extern std::atomic<float> audio_buffer;
extern bool exit_flag;

extern int vis_thread();
extern int aud_thread();



#endif
