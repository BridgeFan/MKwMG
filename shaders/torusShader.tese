#version 420 core
layout( quads ) in;
uniform int SegmentsX;
uniform int SegmentsY;
//unique uniforms
uniform float R, r;
const float M_PI = 3.14159265358;

layout( quads ) in;
uniform mat4 projection; // projection * view * model
uniform mat4 view; // projection * view * model
uniform mat4 model; // projection * view * model
out vec2 t;
void main() {
	float u = gl_TessCoord.x*2.0*M_PI;
	float v = gl_TessCoord.y*2.0*M_PI;
	vec3 p = vec3((R+r*cos(u))*cos(v),(R+r*cos(u))*sin(v), r*sin(u)); //calculate function
	gl_Position = projection * view * model * vec4(p, 1.0);
	t.x=u/(2.0*M_PI);
	t.y=v/(2.0*M_PI);
}
