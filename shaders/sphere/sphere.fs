#version 330 core
out vec4 FragColor;
in vec4 vertexColor;
in vec3 FragPos;

void main(){
   FragColor = vertexColor;
};
