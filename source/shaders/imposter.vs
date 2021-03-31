#version 330 core
layout (location = 0) in vec2 vertex; // <vec2 position, vec2 texCoords>

out vec3 ourColor;

uniform mat4 model;
// note that we're omitting the view matrix; the view never changes so we basically have an identity view matrix and can therefore omit it.
uniform mat4 projection;

void main()
{
    ourColor = vec3(1.0f, 0.0f, 0.0f);
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}