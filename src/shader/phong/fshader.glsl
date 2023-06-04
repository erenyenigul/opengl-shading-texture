#version 410

in vec3 fN, fL, fV;

uniform vec4 AmbientLight, DiffuseLight, SpecularLight;
uniform vec4 AmbientMaterial, DiffuseMaterial, SpecularMaterial;
uniform float Shininess;

out vec4 fcolor;

void main() { 
    vec4 AmbientProduct = AmbientLight*AmbientMaterial;
    vec4 DiffuseProduct = DiffuseLight*DiffuseMaterial;
    vec4 SpecularProduct = SpecularLight*SpecularMaterial;

    // Normalize the input lighting vectors 
    vec3 N = normalize(fN);
    vec3 V = normalize(fV);
    vec3 L = normalize(fL);
    vec3 H = normalize(L + V);
    vec4 ambient = AmbientProduct;
    float Kd = max(dot(L, N), 0.0);
    vec4 diffuse = Kd * DiffuseProduct;
    float Ks = pow(max(dot(N, H), 0.0), Shininess);
    vec4 specular = Ks * SpecularProduct; 

    if(dot(L, N) < 0.1)
        specular = vec4(0.0, 0.0, 0.0, 1.0);
   
    fcolor = ambient + diffuse + specular;
    fcolor.a = 1.0;
}