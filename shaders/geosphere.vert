#version 440 core

layout (location = 1) in vec3 vertex_position;
layout (location = 2) in vec3 vertex_normal;
layout (location = 3) in vec3 vertex_tangent;
layout (location = 4) in vec3 vertex_uv;

layout (location = 1) uniform mat4 projview_mat;
layout (location = 2) uniform mat4 world_transform_mat;

out vec3 vertex_worldposition_fs;
out vec3 vertex_worldnormal_fs;
out vec3 vertex_worldtangent_fs;
out vec3 vertex_uv_fs;

void main() {
    vec4 vpos = vec4(vertex_position, 1.0);
    vertex_worldposition_fs = (world_transform_mat * vpos).xyz;
    vertex_worldnormal_fs = (world_transform_mat * (vpos + vec4(vertex_normal, 1.0)) - vec4(vertex_worldposition_fs, 1.0)).xyz;
    vertex_worldtangent_fs = (world_transform_mat * (vpos + vec4(vertex_tangent, 1.0)) - vec4(vertex_worldposition_fs, 1.0)).xyz;
    vertex_uv_fs = vertex_uv;
    gl_Position = projview_mat * vec4(vertex_worldposition_fs, 1.0);
}