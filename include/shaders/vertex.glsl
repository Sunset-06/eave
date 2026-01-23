R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform float heights[64];
uniform int totalBars;

void main() {
    float height = max(0.02,heights[gl_InstanceID]);
    float availableWidth = 1.9; 
    float barWidth = availableWidth / float(totalBars);
    float startX = -(availableWidth / 2.0);
    float offset = startX + (float(gl_InstanceID) * barWidth);
    
    vec3 pos = aPos;
    
    pos.x *= (barWidth * 0.9 / 0.1);

    if (aPos.y > 0.0) {
        pos.y = height; 
    }

    gl_Position = vec4(pos.x + offset + (barWidth / 2.0), pos.y - 1.0, pos.z, 1.0);
}
)"