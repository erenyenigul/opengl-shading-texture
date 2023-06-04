#version 410

in vec4 vPosition;
in vec3 vNormal; // output values that will be interpolated per-fragment 

out vec3 fN;
out vec3 fV;
out vec3 fL;

uniform mat4 ModelView;
uniform mat4 Transformation;
uniform vec4 LightPosition;
uniform mat4 Projection;

void main() {
    vec3 pos = (ModelView * Transformation * vPosition).xyz;
    
    fN = (transpose(inverse(ModelView * Transformation)) * vec4(vNormal, 0.0)).xyz;
    
    fV = -pos;
    
    fL = (ModelView*LightPosition).xyz;
    
    if((ModelView*LightPosition).w != 0.0)
        fL = (ModelView*LightPosition).xyz - pos;
    
    gl_Position = Projection * ModelView * Transformation * vPosition;
}