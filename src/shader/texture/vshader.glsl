#version 410

in  vec4 vPosition;
in  vec4 vNormal;
in  vec2 vTexCoord;

out vec4 color;
out vec2 texCoord;

uniform mat4 ModelView;
uniform mat4 Transformation;
uniform mat4 Projection;

void main() 
{
    color       = vColor;
    texCoord    = vTexCoord;
    gl_Position = Projection * ModelView * Transformation * vPosition;
} 
