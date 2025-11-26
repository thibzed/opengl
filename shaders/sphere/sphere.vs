#version 330 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 vertexColor;

void main(){
gl_Position = projection * view * model * vec4(Pos, 1.0f);
vertexColor = vec4(Color,1.0f);
};