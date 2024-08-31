#version 400

layout (points) in;
layout (line_strip, max_vertices = 4) out;


uniform float u_Length;


void main()
{

	vec4 a = gl_in[0].gl_Position + vec4(0.0f, u_Length, 0.0f, 0.0f);
	vec4 b = gl_in[0].gl_Position + vec4(u_Length, -u_Length, 0.0f, 0.0f);
	vec4 c = gl_in[0].gl_Position + vec4(-u_Length, -u_Length, 0.0f, 0.0f);
	
	vec4 ac = normalize(c-a);
	vec4 ba = normalize(a-b);
	vec4 cb = normalize(b-c);
	
	//v1 
	gl_Position = a + (ac * u_Length);
	gl_Position = a;
	EmitVertex();
	
	//v2
	gl_Position = b + (ba * u_Length);
	gl_Position = b;
	EmitVertex();
	
	//v3
	gl_Position = c + (cb * u_Length);
	gl_Position = c;
	EmitVertex();
	
	
	gl_Position = a;
	EmitVertex();
	
	EndPrimitive();
	


	//gl_Position = gl_in[0].gl_Position + vec4(-u_Length, 0.0f, 0.0f, 0.0f);
	//EmitVertex();
	
	//gl_Position = gl_in[0].gl_Position + vec4(u_Length, 0.0f, 0.0f, 0.0f);
	//EmitVertex();
	
	//EndPrimitive();
}