#version 330

struct light{
    bool enable;
    vec4 position;
    vec3 ambient, diffuse, specular;
};

const int MAX_LIGHTS = 4;

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

uniform mat4 u_view;
uniform mat4 u_transform;
uniform mat4 u_rotate;
uniform mat4 u_scale;

uniform light u_lights[MAX_LIGHTS];

uniform float u_shininess;
uniform vec3 u_ambient, u_diffuse, u_specular;

varying vec2 v_texcoord;
varying vec3 v_ambient, v_diffuse, v_specular;

light get_color(light l)
{
    mat4 V = u_view * u_scale * u_rotate * u_transform;
    vec3 p = gl_Position.xyz;
    vec3 L = l.position.xyz - p; //light position - point true position
    vec3 E = vec3(0.0, 0.0, 1.0) - p; //eye position - point position
    float dis = 1.0;
    if(l.position.w != 0)
    {
        dis = length(L) + length(E);
        dis *= dis;
    }
    L = normalize(L);
    E = normalize(E);
    vec3 N = normalize((V * vec4(a_normal, 1.0)).xyz);
    float cosT = clamp(dot(N, L), 0.0, 1.0);
    float cosA = clamp(dot(N, normalize(E+L)), 0.0, 1.0);

//    light res;
//    res.ambient = u_ambient * l.ambient;
//    res.diffuse = u_diffuse * l.diffuse * cosT / dis;
//    res.specular = u_specular * l.specular * pow(cosA, u_shininess) / dis;
    l.diffuse *= cosT / dis;
    l.specular *= pow(cosA, u_shininess) / dis;

    return l;
}

void main() {

    gl_Position = u_view * u_scale * u_rotate * u_transform * vec4(a_position, 1.0);

    vec3 ambient = vec3(0.0, 0.0, 0.0), diffuse = vec3(0.0, 0.0, 0.0), specular = vec3(0.0, 0.0, 0.0);
    bool flag = false;
    for(int i = 0; i < MAX_LIGHTS; ++i)
    {
        if(u_lights[i].enable){
            flag = true;
            light res = get_color(u_lights[i]);
            ambient += res.ambient;
            diffuse += res.diffuse;
            specular += res.specular;
        }
    }

    v_ambient = u_ambient;
    v_diffuse = u_diffuse;
    v_specular = u_specular;

//    if(v_ambient == vec3(0.0, 0.0, 0.0)) v_ambient = vec3(0.2, 0.2, 0.2);
//    if(v_diffuse == vec3(0.0, 0.0, 0.0)) v_diffuse = vec3(0.8, 0.8, 0.8);
//    if(v_specular == vec3(0.0, 0.0, 0.0)) v_specular = vec3(1.0, 1.0, 1.0);

    if(flag){
        v_ambient *= ambient;
        v_diffuse *= diffuse;
        v_specular *= specular;
    }else
    {
        v_ambient = vec3(0.2, 0.2, 0.2);
        v_diffuse = vec3(0.6, 0.6, 0.6);
        v_specular = vec3(0.0, 0.0, 0.0);
    }
    v_texcoord = a_texcoord;
}
