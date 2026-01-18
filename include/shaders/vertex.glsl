R"(
#version 330 core
    layout (location = 0) in vec3 aPos;
    uniform float spacing = 0.3;
    uniform float heights[16];

    void main() {
        float height = heights[gl_InstanceID];
        float offset = -0.9 + (gl_InstanceID * 0.12);
        
        vec3 pos = aPos;
        if (aPos.y > 0.0) {
            pos.y = height; 
        }

        gl_Position = vec4(pos.x + offset, pos.y - 1.0, pos.z, 1.0);
    }
)"