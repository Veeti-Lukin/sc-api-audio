// Vertex shader could be used to convert coordinates to normalized coordinates
// Otherwise vertex data should just be passed to later stages
#version 330 core
layout (location = 0) in vec3 aPos;

void main() {
    // Special variable used as output of the vertex shader
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}