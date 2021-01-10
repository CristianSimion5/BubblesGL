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
out mat3 TBN;

uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProjection;

uniform bool uIsFloor;

void main(void) {
    if (uIsFloor) {
        vec3 T = normalize(mat3(mModel) * in_Tangent);
        vec3 B = normalize(mat3(mModel) * in_Bitangent);
        Normal = normalize(mat3(mModel) * vec3(0.f, 1.f, 0.f));
        TBN = mat3(T, B, Normal);
    }
    else
        Normal = mat3(mModel) * in_Normal;
    
    FragPos = vec3(mModel * in_Position);

    TexCoord = in_TexCoord;

    gl_Position = mProjection * mView * mModel * in_Position;
} 
 