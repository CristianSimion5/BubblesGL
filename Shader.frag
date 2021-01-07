
// Shader-ul de fragment / Fragment shader  
 
 #version 400

in vec3 FragPos;
in vec3 Normal;

out vec4 out_Color;

uniform mat4 mTranslate;
uniform float radius;
uniform vec3 uColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos;

void main(void) {
    // Ambient
    float ambientStrength = 0.8f;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.f);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 5.f;
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 

    float spec = pow(max(dot(viewDir, reflectDir), 0.f), 256);
    vec3 specular = specularStrength * spec * lightColor;  

    vec3 result = (ambient + diffuse + specular) * uColor;
    out_Color = vec4(result, .4f);
}
 