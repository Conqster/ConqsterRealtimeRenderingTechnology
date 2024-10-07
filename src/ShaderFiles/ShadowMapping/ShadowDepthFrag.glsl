#version 400

in vec4 FragPos;

uniform vec3 u_LightPos;
uniform float u_FarPlane;
uniform bool u_IsOmnidir;

void main()
{
    if(u_IsOmnidir)
    {
                float light_dist = length(FragPos.xyz - u_LightPos);
    
        //remap to [0;1] range
        light_dist = light_dist / u_FarPlane;
    
        gl_FragDepth = light_dist;
    }
    else
    {
        gl_FragDepth = gl_FragCoord.z;
    }
        

}