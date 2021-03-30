#version 330 core
layout (location = 0) in vec2 vertex; // <vec2 position, vec2 texCoords>

out vec3 ourColor;

// note that we're omitting the view matrix; the view never changes so we basically have an identity view matrix and can therefore omit it.
uniform mat4 projection;

void main()
{
    ourColor = vec3(0.0f, 1.0f, 0.0f);
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
}