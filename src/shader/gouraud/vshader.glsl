#version 410

in vec4 vPosition;
in vec3 vNormal;
out vec4 color;

uniform vec4 AmbientLight, DiffuseLight, SpecularLight;
uniform vec4 AmbientMaterial, DiffuseMaterial, SpecularMaterial;
uniform float Shininess;

uniform mat4 ModelView, Projection, Transformation;
uniform vec4 LightPosition;

void main() { 
    vec4 AmbientProduct = AmbientLight*AmbientMaterial;
    vec4 DiffuseProduct = DiffuseLight*DiffuseMaterial;
    vec4 SpecularProduct = SpecularLight*SpecularMaterial;

    vec3 pos = (ModelView * Transformation * vPosition).xyz;
    vec3 L = normalize(LightPosition.xyz - pos); 
    vec3 V = normalize(-pos); // viewer direction v(camera is at origin)
    vec3 H = normalize(L + V); 
    vec3 N = normalize(ModelView * Transformation * vec4(vNormal, 0.0)).xyz; // Compute terms in the illumination equation
    
    vec4 ambient = 0.1*AmbientProduct; // k_a * L_a
    float Kd = max(dot(L, N), 0.0); //set diffuse to 0 if light is behind the surface point
    vec4 diffuse = Kd * DiffuseProduct; // k_d * L_d 
    float Ks = pow(max(dot(N, H), 0.0), Shininess); // ignore specular component if negative 
    vec4 specular = Ks * SpecularProduct; // k_s * L_s //ignore specular component also if light is behind the surface point 
    
    if(dot(L, N) < 0.0)
        specular = vec4(0.0, 0.0, 0.0, 1.0);
    
    gl_Position = Projection * ModelView * Transformation * vPosition;
    color = ambient + diffuse + specular;
}