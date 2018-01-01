#version 330

struct texture{
    bool enable;
    int type;
    float blend;
    sampler2D  texture;
};

const int MAX_TEXTURES = 4;

uniform texture u_textures[MAX_TEXTURES];
uniform float u_alpha;

varying vec2 v_texcoord;
varying vec3 v_ambient, v_diffuse, v_specular;

//vec4 get_texture_color(texture tex)
//{
//    return texture2D(tex.texture, v_texcoord) * tex.blend;
//}

void main() {
    vec3 ambient = vec3(0.0, 0.0, 0.0);
    vec3 diffuse = vec3(0.0, 0.0, 0.0);
    vec3 specular = vec3(0.0, 0.0, 0.0);

    for(int i = 0; i < MAX_TEXTURES; ++i)
    {
        if(u_textures[i].enable)
        {
            vec3 tex_color = texture2D(u_textures[i].texture, v_texcoord).rgb * u_textures[i].blend;
            if(u_textures[i].type == 0)
            {
                ambient += tex_color;
            } else if(u_textures[i].type == 1)
            {
                diffuse += tex_color;
            } else if(u_textures[i].type == 2)
            {
                specular += tex_color;
            }
        }
    }

    if(ambient == vec3(0.0, 0.0, 0.0)) ambient = vec3(1.0, 1.0, 1.0);
    if(diffuse == vec3(0.0, 0.0, 0.0)) diffuse = vec3(1.0, 1.0, 1.0);
    if(specular == vec3(0.0, 0.0, 0.0)) specular = vec3(1.0, 1.0, 1.0);

    ambient *= v_ambient;
    diffuse *= v_diffuse;
    specular *= v_specular;

    gl_FragColor = vec4((ambient + diffuse + specular), u_alpha);
//    gl_FragColor = vec4((v_ambient + v_diffuse + v_specular), u_alpha);
}
