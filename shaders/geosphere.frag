#version 440 core

in vec3 vertex_worldposition_fs;
in vec3 vertex_worldnormal_fs;
in vec3 vertex_worldtangent_fs;
in vec3 vertex_uv_fs;

out vec4 out_color;

void main() {
    out_color = vec4(0.0, 1.0, 0.0, 1.0); // * ((gl_FragCoord.z - 0.5) * 200);
}