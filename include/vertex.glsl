R"(
#version 330 core
    layout (location = 0) in vec3 aPos;
    uniform float inp_val;
    uniform float curr_time;

    void main() {
        float scale = 1.0 + (inp_val * 0.10);
        float s = sin(curr_time*0.1);
        float c = cos(curr_time*0.1);

        mat4 rotation = mat4(
            vec4(c,   s,  0.0, 0.0),
            vec4(-s,  c,  0.0, 0.0),
            vec4(0.0, 0.0, 1.0, 0.0),
            vec4(0.0, 0.0, 0.0, 1.0)
        );

        gl_Position = rotation * vec4(aPos.x * scale, aPos.y * scale, aPos.z, 1.0);
    }
)"