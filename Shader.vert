// Shader-ul de varfuri  
 
 #version 400


layout(location=0) in vec4 in_Position;
layout(location=1) in vec3 in_Normal;
in vec4 in_Color;

out vec4 gl_Position; 
out vec4 ex_Color;
uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProjection;


void main(void) {
   gl_Position = mProjection * mView * mModel * in_Position;
   ex_Color = in_Color;
} 
 