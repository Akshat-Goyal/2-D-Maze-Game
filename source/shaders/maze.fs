#version 330 core
in vec3 ourColor;
in vec3 FragPos;
out vec4 color;

uniform vec3 lightColor;
uniform vec3 lightPos;
// uniform float lightCutOff;
uniform int light;

void main()
{
    float distance = length(FragPos - lightPos) + 1e-8;
    if(light == 0){
        color = vec4(lightColor * (80 / distance) * ourColor, 1.0f);
    }
    else{
        color = vec4(lightColor * ourColor, 1.0f);
    }
    // if(distance < lightCutOff){
    //     color = vec4(lightColor * ourColor, 1.0f);
    // }
    // else{
    //     color = vec4(0.3 * lightColor * ourColor, 1.0f);
    // }
}