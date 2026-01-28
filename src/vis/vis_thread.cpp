#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "core.h"
#include "vis.h"

bool exit_flag = false;


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

    // compiling shaders and creating shader program
    unsigned int shaderProgram = Create_Shader_Program();

    // Binding VAOs, VBOs and EBOs
    Bind_GLObjects();

    int timeLoc = glGetUniformLocation(shaderProgram, "time");
    int gradLoc = glGetUniformLocation(shaderProgram, "bar_gradient");
    int resLoc = glGetUniformLocation(shaderProgram, "resolution");
    int heightsLoc = glGetUniformLocation(shaderProgram, "heights");
    int colorsLoc = glGetUniformLocation(shaderProgram, "barColours");
    int totalBarsLoc = glGetUniformLocation(shaderProgram, "totalBars");
    //int seedLoc = glGetUniformLocation(shaderProgram, "seed");



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
                }
            }
        }
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);    
        glUseProgram(shaderProgram);

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

        glUniform1f(timeLoc, time);

        glUniform3fv(gradLoc, 3, palettes[currPalette]);

        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        glUniform2f(resLoc, (float)w, (float)h);
        
        glUniform1fv(heightsLoc, BARS, smoothHeights);

        //glUniform3fv(colorsLoc, 1, colours);

        glUniform1i(totalBarsLoc, BARS);

        //glUniform1f(seedLoc, randomSeed);

        glBindVertexArray(r1_VAO);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, BARS);
        
        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    //stbi_image_free(data);


    std::cout << GREET << std::endl;
    return 0;
}