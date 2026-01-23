R"(
    #version 330 core
    
    out vec4 FragColor;
    uniform vec3 barColours;

    void main()
    {
        FragColor = vec4(barColours, 1.0);
    }
)"