#version 400
out vec4 FragColour;

float far = 150.0f;
float near = 0.1f;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0f - 1.0f;
	return (2.0f * near * far) /(far + near - z * (far - near));
}
void main()
{
	float linear_depth = LinearizeDepth(gl_FragCoord.z) /far;
	FragColour = vec4(vec3(linear_depth), 1.0f);
}
