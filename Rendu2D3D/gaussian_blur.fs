#version 120

varying vec2 v_texcoords;

uniform float u_deviation;
uniform int u_samples;
uniform float u_blurSize;
uniform sampler2D u_sampler;

#define PI 3.141592
#define E 2.7182

void main()
{
    float dev = 0.02;
    int samples = 16;
    float blurSize = 0.02;
    if(dev == 0){
        gl_FragColor = texture2D(u_sampler, v_texcoords);
    }
    else
    {
    vec4 col = vec4(0, 0, 0, 0);
    float sum = 0;

    for(int index = 0; index < samples; index++)
    {
        float offset = (index / (samples - 1) - 0.5) * blurSize;
        vec2 uv = v_texcoords + vec2(0.0, offset);

        float stDevSquared = dev * dev;
        float gauss = (1 - sqrt(2 * PI * stDevSquared)) * pow(E, -((offset * offset) / 2 * stDevSquared));
        sum += gauss;
        col += texture2D(u_sampler, uv) * gauss;
    }

    col = col / sum;

    gl_FragColor = col;
    }
}

