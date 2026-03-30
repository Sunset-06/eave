#include <string>
#include <cstdio>
#include <memory>
#include "core.h"
#include "vis.h"

Metadata curr_metadata;

int inf_thread() {
    // Open pipe to the python script
    std::cout << "Started inf thread" << std::endl;
    std::string command = "python3 " + std::string(LISTENER_SCRIPT);
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        perror("popen failed");
        return 1;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::string line(buffer);
        if (line.empty() || line == "\n") continue;

        // split by |
        std::stringstream ss(line);
        std::string t, ar, al, p;

        if (std::getline(ss, t, '|') &&
            std::getline(ss, ar, '|') &&
            std::getline(ss, al, '|') &&
            std::getline(ss, p)) {
            
            std::lock_guard<std::mutex> lock(curr_metadata.mtx);
            curr_metadata.title = t;
            curr_metadata.artist = ar;
            curr_metadata.album = al;
            curr_metadata.path = p;
        }

        if (!p.empty()) {
            int w, h, channels;
            unsigned char* data = stbi_load(p.c_str(), &w, &h, &channels, 4);

            if (data) {
                std::lock_guard<std::mutex> lock(curr_metadata.mtx);
                
                if (curr_metadata.pixel_buffer) stbi_image_free(curr_metadata.pixel_buffer);

                curr_metadata.pixel_buffer = data;
                curr_metadata.w = w;
                curr_metadata.h = h;
                curr_metadata.path = p;
                curr_metadata.title = t;
                curr_metadata.flag = true;
            }
        }
    }
    std::cout << "Closed inf thread" << std::endl;
    pclose(pipe);
    return 0;
}