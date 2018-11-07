#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uvs;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
out vec2 textureCoords;
//out vec2 uv;
void main(void)
{
    vec4 pos = ProjectionMatrix * ModelViewMatrix * vec4(position, 1.0);    
    gl_Position = pos.xyww;
    textureCoords = uvs;
    // float m = max(pos.x,pos.y);
    // m = max(m,pos.z);
    // pos = normalize(pos);
    // if(m == pos.x)
    //     textureCoords = pos.yz/pos.w;

    // if(m == pos.y)
    //     textureCoords = pos.xz/pos.w;

    // if(m == pos.z)
    //     textureCoords = pos.xy/pos.w;
}
