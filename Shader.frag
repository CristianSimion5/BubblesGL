
// Shader-ul de fragment / Fragment shader  
 
 #version 400

out vec4 out_Color;

uniform mat4 mTranslate;
uniform float radius;
uniform vec3 uColor;

void main(void) {
    out_Color = vec4(uColor, 0.6f);
}
 