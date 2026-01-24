#include "core.h"
#include "vis.h"

// Shader imports
const char *vertexShaderSrc = 
#include "shaders/vertex.glsl"
"";

const char *fragShaderSrc = 
#include "shaders/fragment.glsl"
"";

float rect[] = {
     0.05f,  1.0f, 0.0f,  // top right
     0.05f,  0.0f, 0.0f,  // bottom right
    -0.05f,  0.0f, 0.0f,  // bottom left
    -0.05f,  1.0f, 0.0f   // top left
};

unsigned int rect_indices[] = {
    0, 1, 3,
    1, 2, 3
};

// float colours[] = {0.81, 0.23, 0.64};
float colours[] = {0.02, 0.67, 0.10};

unsigned int r1_VBO;
unsigned int r1_VAO;
unsigned int r1_EBO;

float currHeights[BARS] = {0.0f};
float smoothHeights[BARS] = {0.0f};
float velocity[BARS] = {0.0f};

// Setup SDL for OpenGL
void setup(){
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // OpenGL 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // VSync
    SDL_GL_SetSwapInterval(1);
}

// Makes the Entire window Draggable 
SDL_HitTestResult DraggableCallback(SDL_Window* win, const SDL_Point* area, void* data) {
    int w, h;
    SDL_GetWindowSize(win, &w, &h);
    return SDL_HITTEST_DRAGGABLE;
}

// Sets the window up
SDL_Window* Start_Window(SDL_Window* window){
    window = SDL_CreateWindow(
        "Look at those bars go",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN |  SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS
    );
    SDL_SetWindowHitTest(window, DraggableCallback, NULL);


    if (window == NULL) {
        printf("Window failed! SDL Error: %s\n", SDL_GetError());
        SDL_Quit(); 
        return NULL;
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
        return NULL;
    }

    std::cout << "Glad Initialized\n";
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    return window;
}

// Compiles and creates a shader program
unsigned int Create_Shader_Program(){
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
    return shaderProgram;
}

void Bind_GLObjects(){
    // creating buffer vbo and assigning id 1
    glGenBuffers(1, &r1_VBO);
    glGenBuffers(1, &r1_EBO);
    glGenVertexArrays(1, &r1_VAO);

    glBindVertexArray(r1_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, r1_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r1_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rect_indices), rect_indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  
}