#version 440 core

layout (location = 1) in vec3 vertex_position;
layout (location = 2) in vec3 vertex_normal;
layout (location = 3) in vec3 vertex_tangent;
layout (location = 4) in vec3 vertex_uv;

layout (location = 1) uniform mat4 projview_mat;
layout (location = 2) uniform mat4 world_transform_mat;

out vec3 vertex_position_tc;
out vec3 vertex_normal_tc;
out vec3 vertex_tangent_tc;
out vec3 vertex_uv_tc;

void main() {
    vertex_uv_tc = vertex_uv * (1200. / 2048.) + vec3(0.5, 0.5, 0.0) / 2048.0;
    vec4 vpos = vec4(vertex_position, 1.0f);
    vertex_position_tc = vertex_position;
    vertex_normal_tc = vertex_normal;
    vertex_tangent_tc = vertex_tangent;
    //vertex_uv_tc = vertex_uv;
    //gl_Position = proj_mat * world_transform_mat * vpos;
    //vertex_worldposition_tc = (world_transform_mat * vpos).xyz;
    //vertex_worldnormal_tc = normalize((world_transform_mat * (vpos + vec4(vertex_normal, 0.0))).xyz - (world_transform_mat * vpos).xyz);
}