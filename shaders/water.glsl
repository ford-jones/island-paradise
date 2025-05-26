uniform float time;

void main ()
{
    // Pixel coordinates
    vec2 uv = fragPosition.zx * vec2(0.07, 0.07);

    // motion
    vec3 motion = cos((time / 2.0) + uv.xxy);

    // Output to screen
    vec4 tuning = vec4(mix(cos(motion * 0.8).y, cos(motion * 0.4000001).y, sin(motion * 2.1411999).x)) * 2.8699999;
    vec4 color = vec4(0.0, 0.3, 0.9, 0.1);
    outFragment = color * tuning;
    
    return;
}