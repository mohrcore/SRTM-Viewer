#version 440 core

in vec3 vertex_uv_fs;

out vec4 out_color;

//layout (location = 3) uniform vec3 view_dir;
//layout (location = 4) uniform vec3 view_up_vector;
layout (location = 5) uniform mat4 inverse_projview_mat;
layout (location = 6) uniform vec3 noise_pos;

// See perlin3d.frag
float perlin3d(vec3);

vec3 viewBase(vec3 v) {
    //return vec3(v.x * cross(view_dir, view_up_vector), v.y * view_up_vector, v.z * view_dir);
    return v;
}

void main() {
    vec2 screenpos = vec2(vertex_uv_fs.x, 1.0 - vertex_uv_fs.y) * 2.0 - 1.0;
    vec3 ray = (inverse_projview_mat * vec4(screenpos, 0.0, 1.0)).xyz;
    vec3 sphere_point = 2.0 * viewBase(normalize(ray));
    float pamt = 0.0;
    for (uint i = 0; i < 8; i++) {
        float f = float(pow(2.0, i));
        float perlin = perlin3d(f * sphere_point + noise_pos);
        pamt += 1/f * perlin;
    }
    out_color = vec4(pamt, pamt, pamt, 1.0) * vec4(0.6196, 0.9137, 0.8, 1.0) + vec4(0.1882, 0.0196, 0.302, 1.0);
}
