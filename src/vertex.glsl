#version 330 core
in vec2 TexCoord;
out vec4 finalColor;
uniform sampler2D ourTexture;
uniform vec4 our_color;

void main()
{
    finalColor = texture(ourTexture, vec2(TexCoord.x, TexCoord.y)) * vec4(our_color.x, our_color.y, our_color.z, 1.0f);
}