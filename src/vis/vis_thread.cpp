#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <core.h>
#include <vis.h>

const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 480;

bool exit_flag = false;

float r1[] = {
     1.0f,  0.1f, 0.0f,  // top right
     1.0f,  0.0f, 0.0f,  // bottom right
    -1.0f,  0.0f, 0.0f,  // bottom left
    -1.0f,  0.1f, 0.0f   // top left
};
unsigned int r1_indices[] = {
    0, 1, 3,
    1, 2, 3
};  

unsigned int r1_VBO;
unsigned int r1_VAO;
unsigned int r1_EBO;
static float smooth_rms = 0.0f;
const float smoothing_factor = 0.10f;
//const float up_smoothing_factor = 0.30f;


    
const char *vertexShaderSrc = 
#include "shaders/vertex.glsl"
"";

const char *fragShaderSrc = 
#include "shaders/fragment.glsl"
"";

void setup(){
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // OpenGL 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    // This means we draw to a hidden back buffer and swap it to the front buffer when done.
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // VSync
    SDL_GL_SetSwapInterval(1);
}


int vis_thread(){
    SDL_Window* window = NULL;
    
    // Initializing SDL - Only Video for now, we'll need audio later
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    setup();
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // Creating the window
    window = SDL_CreateWindow(
        "Look at those bars go",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (window == NULL) {
        printf("Window failed! SDL Error: %s\n", SDL_GetError());
        SDL_Quit(); 
        return 1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (glContext == NULL) {
        printf("Failed to create OpenGL context! SDL Error: %s\n", SDL_GetError());
    }

    // Initialize GLAD with OS specific function
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)){
        std::cout << "Failed to initialize GLAD\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::cout << "Glad Initialized\n";
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Vertex Shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
    glCompileShader(vertexShader);
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    else{
        std::cout << "Vertex Shader compiled\n";
    }

    // Fragment Shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragShaderSrc, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    else{
        std::cout << "Fragment Shader compiled\n";
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);


    // creating buffer vbo and assigning id 1
    glGenBuffers(1, &r1_VBO);
    glGenBuffers(1, &r1_EBO);
    glGenVertexArrays(1, &r1_VAO);

    glBindVertexArray(r1_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, r1_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(r1), r1, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r1_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(r1_indices), r1_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  

    size_t stride = 3 * sizeof(float); 

    // Position attribute (layout location 0): 3 floats, starts at offset 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    SDL_Event e;

    while (!exit_flag) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                exit_flag = true;
            }
        }
        glClearColor(0.0f, 0.35f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);    
        glUseProgram(shaderProgram);

        float current_rms;

        while (shared_buffer.buf_pop(current_rms)) {
            //std::cout<<"popped\n";
        }
        smooth_rms += (current_rms - smooth_rms) * smoothing_factor;

        if(smooth_rms < current_rms)
            smooth_rms += 0.50f;
            //if (smooth_rms > 1.0f) smooth_rms = 0.0f;
        else {
            smooth_rms -= 0.05f;
            if (smooth_rms < 0.0f) smooth_rms = 0.0f;
        }
        int rmsLoc = glGetUniformLocation(shaderProgram, "inp_val");
        glUniform1f(rmsLoc, smooth_rms);

        float timeValue = SDL_GetTicks() / 1000.0f;
        int timeLoc = glGetUniformLocation(shaderProgram, "curr_time");
        glUniform1f(timeLoc, timeValue);

        glBindVertexArray(r1_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    //stbi_image_free(data);


    std::cout << GREET << std::endl;
    return 0;
}

/* 
Rotation impl.:
    float s = sin(curr_time*0.1);
    float c = cos(curr_time*0.1);

    mat4 rotation = mat4(
        vec4(c,   s,  0.0, 0.0),
        vec4(-s,  c,  0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );
*/