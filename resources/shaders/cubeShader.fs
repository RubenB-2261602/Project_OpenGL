#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D Text;

void main()
{
    FragColor=texture(Text, TexCoord);
}