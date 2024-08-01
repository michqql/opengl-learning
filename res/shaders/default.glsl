#define vertex
#version 330 core
layout(location = 0) in vec4 position;
void main() {
      gl_Position = position;
}

#define fragment
#version 330 core
layout(location = 0) out vec4 colour;
uniform vec4 uColour;
void main() {
      colour = uColour;
}