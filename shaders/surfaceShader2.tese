#version 420 core
layout( quads ) in;
//common uniforms
uniform mat4 view;
uniform mat4 projection;
//unique uniforms
uniform vec2 DivisionBegin;
uniform vec2 DivisionEnd;

vec2 lerp(float t) {
	return DivisionBegin+t*(DivisionEnd-DivisionBegin);
}

const mat4 matrix = 1.0/6.0*transpose(mat4(
		1, 4, 1, 0,
		-3,0, 3, 0,
		3,-6, 3, 0,
		-1,3,-3, 1));

vec3 multiplyPseudoMatrix(vec4 left, vec3 P[16], vec4 right) {
	vec3 tmp[4];
	for(int i=0;i<4;i++) {
		tmp[i]=vec3(0.0f);
		for(int j=0;j<4;j++) {
			tmp[i]+=left[j]*P[4*i+j];
		}
	}
	vec3 ret=vec3(.0f);
	for(int i=0;i<4;i++) {
		ret+=tmp[i]*right[i];
	}
	return ret;
}

layout( quads ) in;
uniform mat4 MVP; // projection * view * model
void main() {
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	// Compute basis functions
	float bu[4], bv[4];
	vec3 P[16];
	for(int i=0;i<16;i++) {
		P[i] = gl_in[i].gl_Position.xyz;
	}
	 // Basis functions for u and v
	vec4 uvec = vec4(1.0,u,u*u,u*u*u);
	vec4 vvec = vec4(1.0,v,v*v,v*v*v);
	vec4 pl = uvec*matrix;
	vec4 pr = transpose(matrix)*vvec;
	vec3 p = multiplyPseudoMatrix(pl, P, pr);
	gl_Position = projection * view * vec4(p, 1.0);
}
