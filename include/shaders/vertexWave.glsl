R"(
#version 330 core

uniform float heights[64];
uniform int totalPoints; // Set this to 64
uniform float time;

void main() {
    // gl_VertexID is the index of the point currently being processed (0 to 63)
    int i = gl_VertexID;
    
    // 1. Calculate X: Map 0-63 to the range -1.0 to 1.0
    float x = (float(i) / float(totalPoints - 1)) * 2.0 - 1.0;

    // 2. Get Audio Data
    float audioHeight = heights[i];

    // 3. Create the "Flowing" Wave Logic
    // We use a sine wave based on X and Time so the line "wobbles" 
    // even with static audio.
    float wave = sin(x * 5.0 + time * 3.0) * 0.05;
    
    // To make it flow to "top and bottom," we center it at 0.0
    // The line will oscillate around the middle of the screen.
    float y = audioHeight + wave;

    // Output the position
    gl_Position = vec4(x, y, 0.0, 1.0);
}
)"