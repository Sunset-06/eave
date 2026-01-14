R"(
    #version 330 core
    out vec4 FragColor;

    in vec2 TexCoord;
    uniform sampler2D ourTexture;
    void main()
    {
        FragColor = vec4(0.5, 0.03, 0.7, 1.0);
    }
)"