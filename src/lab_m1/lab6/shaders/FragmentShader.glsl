#version 330

// Input
// TODO(student): Get values from vertex shader

// Output
layout(location = 0) out vec4 out_color; // scriere in framebuffer default, locatia 0 si un singur buffer si variab declarata de tipul out
in vec3 frag_color;

void main()
{
    // TODO(student): Write pixel out color
    out_color = vec4(frag_color, 1); // factor de opacitate al 4 lea, si la pozitii pt inmultire

}
