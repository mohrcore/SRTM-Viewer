#version 440 core

in vec3 vertex_worldposition_fs;
in vec3 vertex_worldnormal_fs;
in vec3 vertex_worldtangent_fs;
in vec3 vertex_uv_fs;

out vec4 out_color;

layout (binding = 0)   uniform sampler2D srtm_tex;
layout (binding = 1)   uniform sampler2D srtm_normal_tex;

//layout (location = 6)  uniform vec3      dirlights[10];
//layout (location = 16) uniform uint      dirlights_count;
layout (location = 6) uniform vec3 light_dir;
layout (location = 7) uniform float light_amt;

#define SHORT_MAX 65 535

vec3 mapToNormalSpace(vec3 v) {
    return v.x * cross(vertex_worldtangent_fs, vertex_worldnormal_fs) + v.y * vertex_worldnormal_fs + v.z * vertex_worldtangent_fs;
}

void main() {
    float ht = texture(srtm_tex, vertex_uv_fs.xy).r * 65536.0;
    vec2 nv = texture(srtm_normal_tex, vertex_uv_fs.xy).rg * 200.0;
    vec3 color;
    if      (ht < 0  )   color = vec3(0.,       0.,        1.); //blue
    else if (ht < 500)   color = vec3(0.,       ht/500,    0.); //->green
    else if (ht < 1000)  color = vec3(ht/500-1, 1.,        0.); //->yellow
    else if (ht < 2000)  color = vec3(1.,       2.-ht/1000,0.); //->red
    else                 color = vec3(1.,       ht/2000-1 ,ht/2000-1);  //->white
    //out_color = vec4(texture(srtm_tex, vertex_uv_fs.xy).r * 3276.0, 0.4275, 0.7529, 1.0);
    //out_color = vec4(color, 1.0);
    //out_color = vec4(normalize(vec3(nv, 1.0)) / 2.0 + vec3(0.5, 0.5, 0.5), 1.0);
    vec3 normal = mapToNormalSpace(normalize(vec3(nv, 1.0)).xzy);
    //out_color = vec4(vec3(vertex_worldtangent_fs) / 2.0 + vec3(0.5, 0.5, 0.5), 1.0);
    //out_color = vec4(vec3(0.5, 0.5, 0.5) + normal / 2.0, 1.0);

    //const vec3 hlight = normalize(vec3(-1.0, -1.0, 0.0)); //Hardcoded light for testing
    const vec3 hambient = vec3(0.0, 0.0, 0.0);
    const vec3 hdiffuse = vec3(1.0, 1.0, 1.0);
    vec3 light_normalized = -normalize(light_dir);
    float cosalpha = clamp(dot(light_normalized, normal), 0., 1.);
    float light_normal_dot = dot(light_normalized, normal);
    vec3 light_normal_proj = clamp(light_normal_dot, 0., 1.) * normal;
    float cosbeta;
/*     if (light_normal_dot > 0.) {
        vec3 light_reflection_dir = normal + hlight + 2.0 * light_normal_proj;
        //Smoothing is used on edges to prevent sharp specular cutoff on
        //smooth curved surfaces with tangents close to light direction
        float stepval = smoothstep(0.0, 0.1, light_normal_dot);
        cosbeta = stepval * clamp(dot(normalize(light_reflection_dir), vvec), 0., 1.);
    } else {
        cosbeta = 0.;
    } */
    out_color = vec4(light_amt * cosalpha * color + (1.0 - light_amt) * color, 1.0);
}