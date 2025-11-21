#include <main.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool exit_flag = false;


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

    SDL_Event e;

    while (!exit_flag) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                exit_flag = true;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << GREET << std::endl;
    return 0;
}