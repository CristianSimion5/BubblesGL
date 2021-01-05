
// Shader-ul de fragment / Fragment shader  
 
 #version 400

in vec4 ex_Color;
out vec4 out_Color;

uniform mat4 mTranslate;
uniform float uWidth;
uniform float uHeight;
uniform float radius;
uniform vec3 uColor;

void main(void) {
    out_Color = ex_Color;
}
 