#version 330

struct texture{
    bool enable;
    int type;
    float blend;
    sampler2D  texture;
};

const int MAX_TEXTURES = 6;

uniform texture u_textures[MAX_TEXTURES];
uniform float u_alpha;

varying vec2 v_texcoord;
varying vec3 v_ambient, v_diffuse, v_specular;

//vec4 get_texture_color(texture tex)
//{
//    return texture2D(tex.texture, v_texcoord) * tex.blend;
//}

void main() {
    vec4 ambient = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 diffuse = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 specular = vec4(0.0, 0.0, 0.0, 0.0);

    bool flag = false;
    for(int i = 0; i < MAX_TEXTURES; ++i)
    {
        if(u_textures[i].enable)
        {
            flag = true;
            vec4 tex_color = texture2D(u_textures[i].texture, v_texcoord) * u_textures[i].blend;
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

    if(flag)
    {
        ambient *= vec4(v_ambient, u_alpha);
        diffuse *= vec4(v_diffuse, u_alpha);
        specular *= vec4(v_specular, u_alpha);
    }else
    {
        ambient = vec4(v_ambient, u_alpha);
        diffuse = vec4(v_diffuse, u_alpha);
        specular = vec4(v_specular, u_alpha);
    }

    gl_FragColor = ambient + diffuse + specular;
//    gl_FragColor = vec4((v_ambient + v_diffuse + v_specular), u_alpha);
}
