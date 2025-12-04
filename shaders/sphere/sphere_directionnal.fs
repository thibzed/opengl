#version 330 core
in vec4 vertexColor;
in vec3 FragNormal;
in vec3 FragPos;

out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
  
uniform vec3 viewPos;
uniform Light light;
uniform float shininess;
uniform float specularStrenght;

void main()
{
    //ambient
    vec3 ambient = light.ambient * vertexColor.xyz;

    //diffuse
    vec3 norm = normalize(FragNormal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm,lightDir),0.0);
    vec3 diffuse = light.diffuse * diff * vertexColor.xyz;

    //Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir),0.0),shininess);
    vec3 specular = light.specular * spec * specularStrenght;

    float distance = length(FragPos - light.position);
    float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    //vec3 result = attenuation * (ambient + diffuse + specular);
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}