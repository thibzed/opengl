#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main()
{
   FragPos = vec3(model * vec4(aPos, 1.0f));
   //Normal = aNormal; This line is enough if we're not scaling the object.
   //If scaling is applied. Next line is required.
   Normal = mat3(transpose(inverse(model))) * aNormal;
   gl_Position = model * projection * view * vec4(aPos,1.0f);
}