// Shader-ul de fragment / Fragment shader  
 
 #version 400

in vec3 Normal;
in vec2 TexCoord;
in vec3 tangentLightPos;
in vec3 tangentCameraPos;
in vec3 tangentFragPos;

out vec4 out_Color;

uniform vec3 uColor;
uniform vec3 lightColor;
uniform bool uIsFloor;

uniform sampler2D uTexture;
uniform sampler2D uNormalMap;

void main(void) {
    vec3 normal;
    float ambientStrength;
    float specularStrength;
    float shininess;

    if (uIsFloor) {
        normal = texture(uNormalMap, TexCoord).rgb;
        normal = normal * 2.f - 1.f;
        
        ambientStrength = 0.1f;
        specularStrength = 1.f;
        shininess = 128;
    }
    else {
        normal = Normal;
        ambientStrength = 0.8f;
        specularStrength = 5.f;
        shininess = 256;
    }

    // Ambient
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(tangentLightPos - tangentFragPos);

    float diff = max(dot(norm, lightDir), 0.f);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 viewDir = normalize(tangentCameraPos - tangentFragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 

    float spec = pow(max(dot(viewDir, reflectDir), 0.f), shininess);
    vec3 specular = specularStrength * spec * lightColor;  
    
    vec4 result;
    if (uIsFloor) {
        result.rgb = (ambient + diffuse + specular) * texture(uTexture, TexCoord).rgb;
        result.a = 1.f;
    }
    else {
        result.rgb = (ambient + diffuse + specular) * uColor;
        result.a = .75f;
    }
    
    out_Color = result;
}
 