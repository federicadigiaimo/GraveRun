#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text_texture;
uniform vec3 textColor;

void main()
{
    // Prendiamo il valore di "luminosità" del carattere dalla texture
    float alpha = texture(text_texture, TexCoords).r;

    // Disegniamo il pixel solo se non è completamente trasparente
    color = vec4(textColor, alpha);
}
