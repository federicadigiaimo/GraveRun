#version 330 core
layout (location = 0) in vec2 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec2 aTexCoord; // the color variable has attribute position 1
  
out vec4 TexCoords; // output a color to the fragment shader

uniform mat4 Projection;  
uniform mat4 Model;

void main()
{
     TexCoords = aTexCoord;
     gl_Position = Projection * Model * vec4(aPos, aPos.y, 0.0, 1.0);
}  
