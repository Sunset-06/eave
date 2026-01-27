R"(
#version 330 core

out vec4 FragColor;

uniform vec3 bar_gradient[3]; 
uniform vec2 resolution;
uniform float time;

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    
    // creates vertical, horizontal and diagonal waves that interfere with each other
    float brightness = sin(uv.x * 3.0 + time * 0.2) +
                       sin(uv.y * 7.0 - time * 0.3) +
                       sin((uv.x + uv.y) * 3.0 + time * 0.5);

    // Normalize from -3.0 - 3.0 to 0.0 to 1.0
    brightness = (brightness + 3.0) / 6.0;

    float accentMask = smoothstep(0.1, 0.6, brightness);
    float midToneMask = smoothstep(0.3, 0.9, brightness);

    // layering colors
    vec3 finalCol = bar_gradient[0];

    finalCol = mix(finalCol, bar_gradient[1], midToneMask);

    finalCol = mix(finalCol, bar_gradient[2], accentMask);

    FragColor = vec4(finalCol, 1.0);
}
)"