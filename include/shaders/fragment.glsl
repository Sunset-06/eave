R"(
#version 330 core

out vec4 FragColor;

uniform vec3 bar_gradient[3]; 
uniform vec2 resolution;
uniform float time;

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    const float scale = 4.0;
    float speed = time * 0.1;
    const float stretch = 0.3;
    
    vec2 midstep = uv;
    midstep.x += 0.8 * sin(uv.y * scale + speed * 0.5);
    midstep.y += 0.8 * sin(uv.x * scale * stretch + speed * 0.5);

    // creates vertical, horizontal and diagonal waves that interfere with each other
    float brightness = 0.8 * sin(midstep.x * 3.0 + speed * 0.2) +
                       sin(midstep.y * 5.0 - speed * 0.3) +
                       sin((midstep.x + midstep.y) * 3.0 + speed); 

    // Normalize from -3.0 - 3.0 to 0.0 to 1.0
    brightness = pow((brightness + 3.0) / 6.0, 1.5);

    brightness = clamp(brightness, 0.0, 1.0);

    vec3 finalCol;
    if (brightness < 0.5) {
        finalCol = mix(bar_gradient[0], bar_gradient[1], brightness * 2.0);
    } else {
        finalCol = mix(bar_gradient[1], bar_gradient[2], (brightness - 0.5) * 2.0);
    }
    FragColor = vec4(finalCol, 1.0);
}
)"