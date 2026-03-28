#ifndef VIS_H
#define VIS_H

#include "glad/glad.h"
#include "stb_image.h"
#include <SDL2/SDL.h>
#include <fcntl.h>
#include <unistd.h>

const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 480;

// Movement Modifiers
const float accel_up = 0.5f;
const float accel_down = 0.03f;
const float friction = 0.5f;
const float sensitivity = 0.8f;

// Bar vertices, indexes and colours
extern float rect[12];
extern unsigned int rect_indices[6];
//extern float colours[3];
extern float palettes[][9];
// palettes
extern int currPalette;
extern int totalPalettes;
//extern float randomSeed;

// bar objects 
extern unsigned int r1_VBO;
extern unsigned int r1_VAO;
extern unsigned int r1_EBO;

// cover objects
extern unsigned int coverVAO;
extern unsigned int coverVBO;
extern unsigned int coverEBO;

extern GLuint coverTex;
extern std::string lastPath;
extern int coverLoc;
// height data
extern float currHeights[BARS];
extern float smoothHeights[BARS];
extern float velocity[BARS];

extern void setup();
extern SDL_HitTestResult DraggableCallback(SDL_Window* win, const SDL_Point* area, void* data);
extern SDL_Window* Start_Window(SDL_Window* window);
extern unsigned int Create_Shader_Program(const char* vertexShaderSrc , const char *fragShaderSrc);
extern GLuint loadTexture(const char* path);
extern void Bind_GLObjects();
extern void Bind_CoverObjects();

// Shader imports
extern const char *barsVertexShaderSrc;
extern const char *waveVertexShaderSrc;
extern const char *defFragShaderSrc;

extern const char *coverFragShaderSrc;
extern const char *coverVertexShaderSrc;

#endif 