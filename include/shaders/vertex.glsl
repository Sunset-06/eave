R"(
#version 330 core
    layout (location = 0) in vec3 aPos;
    uniform float offset;
    uniform float width;
    uniform float height;
    void main() {
        vec3 pos = aPos;
        pos.y *= height;
        pos.x = (pos.x * width) + offset;
        
        gl_Position = vec4(pos, 1.0);
    }
)"