#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D Text;

void main()
{
    float ambientStrenght = 0.3;
    vec4 ambient = ambientStrenght * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = diff * lightColor;

    float specularStrenght = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec4 specular = (specularStrenght * spec * lightColor); 

    vec4 result = (ambient + diffuse + specular) * texture(Text, TexCoord);
    FragColor = result;
}