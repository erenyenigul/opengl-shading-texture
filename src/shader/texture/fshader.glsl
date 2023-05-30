#version 410

in  vec4 color;
in  vec2 texCoord;
out vec4 fcolor;

uniform sampler2D tex;

void main() 
{ 
    fcolor = texture(tex, texCoord);
} 

