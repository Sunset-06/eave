#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vis.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool exit_flag = false;

float t1[] = {
    -0.5f,  0.3f, 0.0f,
    -0.3f,  0.0f, 0.0f,
    -0.9f, -0.5f, 0.0f 
};

float t2[] = {
    // Vertex coords   // Tex coords
    0.9f,  0.7f, 0.0f, 0.0f, 0.0f,
    0.1f,  0.5f, 0.0f, 1.0f, 0.0f,
    0.3f, -0.7f, 0.0f, 0.5f, 1.0f 
};

float texCoords[] = {
    0.0f, 0.0f,  // lower-left corner  
    1.0f, 0.0f,  // lower-right corner
    0.5f, 1.0f   // top-center corner
};

unsigned int indices[] = {
    0, 1, 2
};

unsigned int t1_VBO;
unsigned int t1_VAO;
unsigned int t1_EBO;


unsigned int t2_VBO;
unsigned int t2_VAO;
unsigned int t2_EBO;
    
const char* vertexShader = 
#include "vertex.glsl"
"";

const char *fragShader = 
#include "fragment.glsl"
"";

void setup(){
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Request an OpenGL 3.3 Context (a common starting point)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // Set the size of the buffer components (e.g., 8 bits for Red, Green, Blue, Alpha)
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    // Set double buffering (Crucial for smooth animation)
    // This means we draw to a hidden back buffer and swap it to the front buffer when done.
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}


int main(){
    SDL_Window* window = NULL;
    
    // Iitializing SDL - Only Video for now, we'll need audio later
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    setup();
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // Creating the window
    window = SDL_CreateWindow(
        "Hello",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
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
    glShaderSource(vertexShader, 1, &vertexShader, NULL);
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
    glShaderSource(fragmentShader, 1, &fragShader, NULL);
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

    // Texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char *data = stbi_load("tex/texture.jpeg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);


    // creating buffer vbo and assigning id 1
    glGenBuffers(1, &t1_VBO);
    glGenBuffers(1, &t1_EBO);
    glGenVertexArrays(1, &t1_VAO);

    glGenBuffers(1, &t2_VBO);
    glGenBuffers(1, &t2_EBO);
    glGenVertexArrays(1, &t2_VAO);

    
    glBindVertexArray(t1_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, t1_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(t1), t1, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t1_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  

    glBindVertexArray(t2_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, t2_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(t2), t2, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t2_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    size_t stride = 5 * sizeof(float); 

    // Position attribute (layout location 0): 3 floats, starts at offset 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // Texture attribute (layout location 1): 2 floats, starts at offset 3 * sizeof(float)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

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

        glBindVertexArray(t1_VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t1_EBO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(t2_VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t1_EBO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        
        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    stbi_image_free(data);


    std::cout << GREET << std::endl;
    return 0;
}