#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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
#include "shaders/fragment.glsl"
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
    
    unsigned int barProgram = Create_Shader_Program(barsVertexShaderSrc, defFragShaderSrc);
    unsigned int waveProgram = Create_Shader_Program(waveVertexShaderSrc, defFragShaderSrc);

    // Cache the uniform locations for Bars
    int bTimeLoc    = glGetUniformLocation(barProgram, "time");
    int bGradLoc    = glGetUniformLocation(barProgram, "bar_gradient");
    int bResLoc     = glGetUniformLocation(barProgram, "resolution");
    int bHeightsLoc = glGetUniformLocation(barProgram, "heights");
    int bTotalBarsLoc = glGetUniformLocation(barProgram, "totalBars");

    // Cache the uniform locations for Wave
    int wTimeLoc         = glGetUniformLocation(waveProgram, "time");
    int wGradLoc         = glGetUniformLocation(waveProgram, "bar_gradient");
    int wResLoc          = glGetUniformLocation(waveProgram, "resolution");
    int wHeightsLoc      = glGetUniformLocation(waveProgram, "heights");
    int wTotalPointsLoc  = glGetUniformLocation(waveProgram, "totalPoints");

    Bind_GLObjects();

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
        
        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    //stbi_image_free(data);


    std::cout << GREET << std::endl;
    return 0;
}