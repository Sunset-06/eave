R"(
#version 330 core

uniform float heights[64];
uniform int totalPoints;
uniform float time;

void main() {
    int i = gl_VertexID;
    float x = (float(i) / float(totalPoints - 1)) * 2.0 - 1.0;

    float scale = 0.3;
    float audioHeight = heights[i] * scale;
    // We use a sine wave based on X and time so the line wobbles
    float wave = sin(x * 5.0 + time * 3.0) * 0.05;
    float mirror = (gl_InstanceID == 0) ? 1.0 : -1.0;
    
    // The line will oscillate around the middle of the screen.
    float y = (audioHeight + wave) * mirror + (0.2 * mirror);

    gl_Position = vec4(x, y, 0.0, 1.0);
}
)"