#version 330 core
layout (location = 0) in vec2 vertex;

out vec4 ParticleColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 color;

void main(){
    float scale = 1.0f;
    ParticleColor = color;
    //ParticleColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    gl_Position = projection * view * model * vec4((vertex.xy * scale), 0.0f, 1.0f);
}