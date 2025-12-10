#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture1;

out vec4 FragColor;

void main (){
    vec4 texColor = texture(texture1, TexCoords);
    if (texColor.a < 0.05) discard;
    FragColor = texColor;
}