R"(#version 330 core

out vec4 FragColor;

uniform vec3 bar_gradient[3]; 
uniform vec2 resolution;
uniform float time;

// THIS IS FOR THE COVER PALETTE
uniform sampler2D coverArt;
uniform bool useTexture;

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    float speed = time * 0.1;
    
    vec2 warp = uv;
    warp.x += 0.5 * sin(uv.y * 3.5 + speed);
    warp.y += 0.5 * cos(uv.x * 2.8 + speed * 0.5);
    
    float b = 0.5 * sin(warp.x * 4.0 + speed);
    b += 0.5 * cos(warp.y * 3.0 - speed * 1.2);
    
    // Radial
    vec2 center = uv - vec2(0.5);
    float dist = length(center) * 10.0;
    b += 0.4 * sin(dist - speed * 2.0);
    
    b += 0.2 * sin((warp.x + warp.y) * 8.0 + speed);

    float brightness = (b + 1.6) / 3.2;
    
    brightness = pow(clamp(brightness, 0.0, 1.0), 1.8);

    // Blending
    vec3 finalCol;

    if (useTexture) {
        finalCol = texture(coverArt, uv).rgb;
        
        finalCol *= (brightness + 0.3);
    }
    else{
        if (brightness < 0.5) {
            finalCol = mix(bar_gradient[0], bar_gradient[1], brightness * 2.0);
        } else {
            float t = smoothstep(0.0, 1.0, (brightness - 0.5) * 2.0);
            finalCol = mix(bar_gradient[1], bar_gradient[2], t);
        }
    }
    
    FragColor = vec4(finalCol, 1.0);
})"