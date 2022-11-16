#version 330

// Input
// TODO(student): Get vertex attributes from each location

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
// variabile uniforme, variab specifice geometriei obiect

// Output
// TODO(student): Output values to fragment shader
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_color;
layout(location = 3) in vec3 v_normalise;
layout(location = 2) in vec2 v_tx_coord;

out vec3 frag_color;

void main()
{
    // TODO(student): Send output to fragment shader
    
    frag_color = v_color;

    // TODO(student): Compute gl_Position
    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
    // poz finala a vertexului

}
