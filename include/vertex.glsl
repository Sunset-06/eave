R"(
#version 330 core
    layout (location = 0) in vec3 aPos;
    uniform float inp_val;
    uniform float curr_time;

    void main() {
        float scale = 1.0 + (inp_val * 0.10);

        gl_Position = vec4(aPos.x, (aPos.y * scale) - 1.0, aPos.z, 1.0);
    }
)"