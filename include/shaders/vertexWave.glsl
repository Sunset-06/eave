R"(
#version 330 core

uniform float heights[64];
uniform int totalPoints;
uniform float time;

void main() {
    int isBottom = gl_VertexID % 2;
    int i = gl_VertexID / 2;

    float x = (float(i) / float(totalPoints - 1)) * 2.0 - 1.0;

    float scale = 0.4;
    float audioHeight = heights[i] * scale;
    // We use a sine wave based on X and time so the line wobbles
    float wave = sin(x * 5.0 + time * 3.0) * 0.05;
    float offset = 0.1;
    
    float y;
    if (isBottom == 0) y = (audioHeight + wave) + offset;
    else y = -(audioHeight + wave) - offset;

    gl_Position = vec4(x, y, 0.0, 1.0);
}
)"