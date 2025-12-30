#version 330 core
layout (location = 0) in vec2 vertex;

out vec4 ParticleColor;

uniform mat4 projection;
uniform vec4 color;

void main(){
    float scale = 10.0f;
    ParticleColor = color;
    gl_Position = projection * vec4((vertex.xy * scale), 0.0f, 1.0f);
}