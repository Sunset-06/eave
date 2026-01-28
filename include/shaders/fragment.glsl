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
    midstep.x += 0.8 * sin(uv.y * scale + speed);
    midstep.y += 0.8 * sin(uv.x * scale * stretch + speed * 0.8);

    // creates vertical, horizontal and diagonal waves that interfere with each other
    float brightness = 0.8 * sin(midstep.x * 3.0 + speed * 0.2) +
                       sin(midstep.y * 7.0 - speed * 0.3) +
                       sin((midstep.x + midstep.y) * 3.0 + speed); 

    // Normalize from -3.0 - 3.0 to 0.0 to 1.0
    brightness = (brightness + 3.0) / 6.0;

    float accentMask = smoothstep(0.1, 0.5, brightness);
    float midToneMask = smoothstep(0.3, 1.0, brightness);

    // layering colors
    vec3 finalCol = bar_gradient[0];

    finalCol = mix(finalCol, bar_gradient[1], midToneMask);

    finalCol = mix(finalCol, bar_gradient[2], accentMask);

    FragColor = vec4(finalCol, 1.0);
}
)"