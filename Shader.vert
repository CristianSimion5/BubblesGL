// Shader-ul de varfuri  
 
 #version 400

layout(location=0) in vec4 in_Position;
layout(location=1) in vec3 in_Normal;
layout(location=2) in vec2 in_TexCoord;
layout(location=3) in vec3 in_Tangent;
layout(location=4) in vec3 in_Bitangent;

out vec4 gl_Position; 
out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;
out vec3 tangentLightPos;
out vec3 tangentCameraPos;
out vec3 tangentFragPos;

uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProjection;
uniform vec3 lightPos;
uniform vec3 cameraPos;

uniform bool uIsFloor;

void main(void) {
    mat3 TBN = mat3(1.f);
    if (uIsFloor) {
        vec3 T = normalize(mat3(mModel) * in_Tangent);
        vec3 B = normalize(mat3(mModel) * in_Bitangent);
        Normal = normalize(mat3(mModel) * vec3(0.f, 1.f, 0.f));
        TBN = transpose(mat3(T, B, Normal));
    }
    else
        Normal = mat3(mModel) * in_Normal;
    
    tangentLightPos = TBN * lightPos;
    tangentCameraPos = TBN * cameraPos;
    tangentFragPos = TBN * vec3(mModel * in_Position);

    TexCoord = in_TexCoord;

    gl_Position = mProjection * mView * mModel * in_Position;
} 
 