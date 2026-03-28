#define STB_IMAGE_IMPLEMENTATION
#include "core.h"
#include "vis.h"

bool exit_flag = false;

const char *barsVertexShaderSrc =
#include "shaders/vertexBars.glsl"
"";

const char *waveVertexShaderSrc =
#include "shaders/vertexWave.glsl"
"";

const char *defFragShaderSrc =
#include "shaders/newfrag.glsl"
"";

const char *coverVertexShaderSrc =
#include "shaders/covervrtx.glsl"
"";

const char *coverFragShaderSrc =
#include "shaders/coverfrag.glsl"
"";


int vis_thread(){    
    // Initializing SDL - Only Video for now, we'll need audio later
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    //setting up SDL with OpenGL
    setup();

    // Creating the window
    SDL_Window* window = Start_Window(window);

    if (window == NULL) {
        printf("Window failed! SDL Error: %s\n", SDL_GetError());
        SDL_Quit(); 
        return 1;
    }

    // starting the metadata script
    FILE* pipe = popen("python3 ../listen.py", "r");
    if (!pipe) {
        std::cerr << "Failed to start Python script\n";
    }

    int fd = fileno(pipe);
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    
    unsigned int barProgram = Create_Shader_Program(barsVertexShaderSrc, defFragShaderSrc);
    unsigned int waveProgram = Create_Shader_Program(waveVertexShaderSrc, defFragShaderSrc);
    unsigned int coverProgram = Create_Shader_Program(coverVertexShaderSrc, coverFragShaderSrc);

    // Cache the uniform locations for Bars
    int bTimeLoc      = glGetUniformLocation(barProgram, "time");
    int bGradLoc      = glGetUniformLocation(barProgram, "bar_gradient");
    int bResLoc       = glGetUniformLocation(barProgram, "resolution");
    int bHeightsLoc   = glGetUniformLocation(barProgram, "heights");
    int bTotalBarsLoc = glGetUniformLocation(barProgram, "totalBars");

    // Cache the uniform locations for Wave
    int wTimeLoc         = glGetUniformLocation(waveProgram, "time");
    int wGradLoc         = glGetUniformLocation(waveProgram, "bar_gradient");
    int wResLoc          = glGetUniformLocation(waveProgram, "resolution");
    int wHeightsLoc      = glGetUniformLocation(waveProgram, "heights");
    int wTotalPointsLoc  = glGetUniformLocation(waveProgram, "totalPoints");

    //Cache for album art
    int coverLoc = glGetUniformLocation(coverProgram, "resolution");

    Bind_GLObjects();
    Bind_CoverObjects();

    SDL_Event e;

    while (!exit_flag) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                exit_flag = true;
            }
            else if (e.type == SDL_KEYDOWN) {
                switch(e.key.keysym.sym){
                    case SDLK_ESCAPE:
                        exit_flag = true;
                        break;
                    case SDLK_p:
                        currPalette = (currPalette + 1) % totalPalettes;
                        std::cout << "Swapped to palette: " << currPalette << std::endl;
                        break;
                    case SDLK_m:
                        current_mode = (current_mode == MODE_BARS) ? MODE_WAVE : MODE_BARS;
                        std::cout << "Switched Mode to: " << current_mode << std::endl;
                        break;
                }
            }
        }

        // check for metadata change
        char buffer[512];
        while(fgets(buffer, sizeof(buffer), pipe) != NULL) {
            std::string line(buffer);
            size_t lastPipe = line.find_last_of('|');
            if (lastPipe != std::string::npos) {
                std::string path = line.substr(lastPipe + 2); // skip "| "
                path.erase(path.find_last_not_of(" \n\r\t") + 1); // trim whitespace

                if (path != lastPath && !path.empty()) {
                    if (coverTex != 0) glDeleteTextures(1, &coverTex);
                    coverTex = loadTexture(path.c_str());
                    lastPath = path;
                    std::cout << "Loaded new art: " << path << std::endl;
                }
            }
        }

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);    
        Frame nextFrame;

        while(shared_buffer.buf_pop(nextFrame)) {
            for(int i = 0; i < BARS; i++) {
                float target = nextFrame.bars[i]*sensitivity;
                float diff = target - smoothHeights[i];

                if(diff > 0){
                    velocity[i] += diff * accel_up;
                }
                else {
                    velocity[i] += diff * accel_down;
                }

                smoothHeights[i] += velocity[i];
                velocity[i] *= friction;

            }
        }

        float time = SDL_GetTicks() / 1000.0f;
        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        switch (current_mode) {
            case MODE_BARS:
                glUseProgram(barProgram);
                glUniform1f(bTimeLoc, time);
                glUniform3fv(bGradLoc, 3, palettes[currPalette]);
                glUniform2f(bResLoc, (float)w, (float)h);
                glUniform1fv(bHeightsLoc, BARS, smoothHeights);
                glUniform1i(bTotalBarsLoc, BARS);

                glBindVertexArray(r1_VAO);
                glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, BARS);
                break;

            case MODE_WAVE:
                glUseProgram(waveProgram);
                glUniform1f(wTimeLoc, time);
                glUniform3fv(wGradLoc, 3, palettes[currPalette]);
                glUniform2f(wResLoc, (float)w, (float)h);
                glUniform1fv(wHeightsLoc, BARS, smoothHeights);
                glUniform1i(wTotalPointsLoc, BARS);

                glBindVertexArray(r1_VAO);
                glLineWidth(2.0f);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, BARS*2);
                break;
        }
        
        if (coverTex != 0) {
            glUseProgram(coverProgram);
            glUniform2f(coverLoc, (float)w, (float)h);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, coverTex);
            
            glBindVertexArray(coverVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        SDL_GL_SwapWindow(window);
    }
    pclose(pipe);
    SDL_DestroyWindow(window);
    SDL_Quit();


    std::cout << GREET << std::endl;
    return 0;
}