#version 400

out vec4 FragColour;

in vec2 v_UVCoord;

uniform samplerCube uLightShadowMap;
uniform float uFar;
uniform float uNear;
uniform int uFaceIdx; //0>>5




//Helper function 
vec3 GetFaceDircetion(int face, vec2 uv)
{
    //transform UV coord from [0;1] to [-1;1]
    vec3 dir;
    if(face == 0) 
        dir = vec3(1.0f, uv.y, -uv.x); //+X 
    else if(face == 1)
        dir = vec3(-1.0f, uv.y, uv.x); //-X
    else if (face == 2)
        dir = vec3(uv.x, 1.0f, -uv.y); //+Y
    else if (face == 3)
        dir = vec3(uv.x, -1.0f, uv.y); //-Y
    else if (face == 4) 
        dir = vec3(uv.x, uv.y, 1.0f); //+Z
    else
        dir = vec3(-uv.x, uv.y, -1.0f); //-Z
        
    //return dir;
    return normalize(dir);
}

float LineariseDepth(float depth)
{
	float z = depth * 2.0f - 1.0f;
	return (2.0f * uNear * uFar) / (uFar + uNear - z * (uFar - uNear));
}


void main()
{
    //vector direction corresponding to cube mapp face
    vec3 face_dir = GetFaceDircetion(uFaceIdx, v_UVCoord);
    //sample depth value from map with direction vector
    float depth_value = texture(uLightShadowMap, face_dir).r;
    //visualize depth [0;1] grayscale
    FragColour = vec4(vec3(LineariseDepth(depth_value) / uFar), 1.0f);
    
}