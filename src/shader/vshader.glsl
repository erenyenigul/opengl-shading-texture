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

void main()
{
    // Transform vertex position into camera (eye) coordinates
    vec3 pos = (ModelView * Transformation * vPosition).xyz;
    
    fN = (ModelView * Transformation*vec4(vNormal, 0.0)).xyz; // normal direction in camera coordinates

    fV = -pos; //viewer direction in camera coordinates

    fL = LightPosition.xyz; // light direction

    if( LightPosition.w != 0.0 ) {
        fL = LightPosition.xyz - pos;  //point light source
    }

    gl_Position = Projection * ModelView * Transformation * vPosition;
}
