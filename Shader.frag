
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
    float dist = distance(vec2(gl_FragCoord.x / uWidth, gl_FragCoord.y / uHeight) * 2.f - 1.f, mTranslate[3].xy);
    if (dist >= radius) {
        discard;
    }
    float a = dist / radius;
    
    out_Color.rgb = uColor;
    out_Color.a = max(2.f * a - 1.f, 0.2f);
}
 