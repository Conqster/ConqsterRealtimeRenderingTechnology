#version 400

layout (points) in;
//layout (line_strip, max_vertices = 7) out;
layout (triangle_strip, max_vertices = 6) out;

out vec4 f_Colour;

//in vec4 v_Colour;

in VS_OUT
{
	vec4 colour;
}gs_in[];


uniform float u_Length;

//////////////////
///////#a////////
//////#/#/////////
////c#####d/////////
/////#///#///////
////a#####b//////
/////////////////


void build_house(vec4 center, float offset_mag)
{
	vec4 a = vec4(-offset_mag, -offset_mag, 0.0f, 0.0f);    
	vec4 b = vec4(offset_mag, -offset_mag, 0.0f, 0.0f);
	vec4 c = vec4(-offset_mag, offset_mag, 0.0f, 0.0f);
	vec4 d = vec4(offset_mag, offset_mag, 0.0f, 0.0f);
	vec4 e = vec4(0.0f, 2.0f * offset_mag, 0.0f, 0.0f);
		
	//f_Colour = v_Colour;
	f_Colour = gs_in[0].colour;
	gl_Position = center + a;
	EmitVertex();
	
	gl_Position = center + b;
	EmitVertex();
	
	gl_Position = center + c;
	EmitVertex();
	
	gl_Position = center + d;
	EmitVertex();
	
	f_Colour = vec4(1.0);
	gl_Position = center + e;
	EmitVertex();
	
	EndPrimitive();
}


void main()
{
	build_house(gl_in[0].gl_Position, u_Length);
	return;
	
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