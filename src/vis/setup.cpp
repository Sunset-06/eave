#include "core.h"
#include "vis.h"


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

float coverVertices[] = {
    -0.95f, -0.50f,  0.0f, 1.0f,
    -0.95f, -0.95f,  0.0f, 0.0f,
    -0.50f, -0.95f,  1.0f, 0.0f,
    -0.50f, -0.50f,  1.0f, 1.0f,
};
unsigned int coverIndices[] = { 0, 1, 2, 0, 2, 3 };

float palettes[][9] = {
    {
     0.01f, 0.73f, 0.59f, // Turquoise
     0.0f, 0.81f, 0.0f,   // Green
     0.1f, 0.674f, 0.18f  // Slightly different green
    },

    {
    1.0f, 0.96f, 0.0f,   // Yellow
    1.0f, 0.66f, 0.07f,  // Orange
    0.67f, 0.09f, 0.16f  // Red
    },

    {
    0.67f, 0.00f, 0.78f,  // Violet
    0.60f, 0.01f, 0.67f,  // Purple
    0.38f, 0.00f, 0.6f,   // Light Purple
    },

    {
    0.20f, 0.00f, 0.10f,  // Maroon
    0.83f, 0.00f, 0.47f,  // Pink
    0.58f, 0.39f, 0.76f,  // Lavender
    },

    {
    0.05f, 0.00f, 0.12f,  // Indigo
    0.10f, 0.15f, 0.65f,  // Cobalt
    0.40f, 0.75f, 1.00f   // Sky Blue
}
};

int currPalette = 0;
int totalPalettes = 5;
//float randomSeed = static_cast<float>(rand() % 10000) / 10.0f;

unsigned int r1_VBO;
unsigned int r1_VAO;
unsigned int r1_EBO;

unsigned int coverVAO;
unsigned int coverVBO;
unsigned int coverEBO;
GLuint coverTex = 0;
std::string lastPath = "";

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
unsigned int Create_Shader_Program(const char* vertexShaderSrc , const char *fragShaderSrc){
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

/// for the album cover
GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); 
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Failed to load texture at: " << path << std::endl;
        return 0;
    }
    return textureID;
}

void Bind_CoverObjects() {
    glGenVertexArrays(1, &coverVAO);
    glGenBuffers(1, &coverVBO);
    glGenBuffers(1, &coverEBO);

    glBindVertexArray(coverVAO);

    glBindBuffer(GL_ARRAY_BUFFER, coverVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coverVertices), coverVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, coverEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(coverIndices), coverIndices, GL_STATIC_DRAW);

    // Position (Location 0)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // TexCoords (Location 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}