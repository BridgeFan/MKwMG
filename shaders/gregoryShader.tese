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
//TODO - everything

void basisFunctions(out vec4 b, float t) {
	float t1 = (1.0 - t);
	float t12 = t1 * t1;
	// Bernstein polynomials
	b[0] = t12 * t1;
	b[1] = 3.0 * t12 * t;
	b[2] = 3.0 * t1 * t * t;
	b[3] = t * t * t;
}

layout( quads ) in;
//uniform mat4 MVP; // projection * view * model
void main() {
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	vec3 P[4], em[4], ep[4], fm[4], fp[4];
	for(int i=0;i<4;i++) {
		P[i]=gl_in[3*i].gl_Position.xyz;
		em[i]=gl_in[(3*i+11)%12].gl_Position.xyz;
		ep[i]=gl_in[3*i+1].gl_Position.xyz;
		fm[i]=gl_in[12+(7+2*i)%8].gl_Position.xyz;
		fp[i]=gl_in[12+2*i].gl_Position.xyz;
	}
	vec3 F0=(u*fp[0]+v*fm[0])/(u+v);
	vec3 F1=((1-u)*fm[1]+v*fp[1])/(1-u+v);
	vec3 F2=((1-u)*fp[2]+(1-v)*fm[2])/(2-u-v);
	vec3 F3=(u*fm[3]+(1-v)*fp[3])/(1+u-v);
	// Compute basis functions
	if(u<0.001 && v<0.001) {
		gl_Position = projection * view * vec4(P[0], 1.0);
		return;
	}
	else if(u>0.999 && v<0.001) {
		gl_Position = projection * view * vec4(P[1], 1.0);
		return;
	}
	else if(u>0.999 && v>0.999) {
		gl_Position = projection * view * vec4(P[2], 1.0);
		return;
	}
	else if(u<0.001 && v>0.999) {
		gl_Position = projection * view * vec4(P[3], 1.0);
		return;
	}
	vec4 Bu, Bv;
	basisFunctions(Bu, u);
	basisFunctions(Bv, v);

	mat4 Gx =
	{{P[0].x,	ep[0].x,em[1].x,P[1].x},
	{em[0].x,	F0.x,	F1.x,	ep[1].x},
	{ep[3].x,	F3.x,	F2.x,	em[2].x},
	{P[3].x,	em[3].x,ep[2].x,P[2].x}};

	mat4 Gy =
	{{P[0].y,	ep[0].y,em[1].y,P[1].y},
	{em[0].y,	F0.y,	F1.y,	ep[1].y},
	{ep[3].y,	F3.y,	F2.y,	em[2].y},
	{P[3].y,	em[3].y,ep[2].y,P[2].y}};

	mat4 Gz =
	{{P[0].z,	ep[0].z,em[1].z,P[1].z},
	{em[0].z,	F0.z,	F1.z,	ep[1].z},
	{ep[3].z,	F3.z,	F2.z,	em[2].z},
	{P[3].z,	em[3].z,ep[2].z,P[2].z}};

	vec3 p;
	p.x = dot(Bu*Gx,Bv);
	p.y = dot(Bu*Gy,Bv);
	p.z = dot(Bu*Gz,Bv);
	gl_Position = projection * view * vec4(p, 1.0);
}
