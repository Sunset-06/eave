R"(
#version 330 core
    layout (location = 0) in vec3 aPos;
    uniform float u_rms;

    void main() {
        float scale = 1.0 + (u_rms * 10.0); 
        gl_Position = vec4(aPos.x * scale, aPos.y * scale, aPos.z, 1.0);
    }
)"