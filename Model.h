//
// Created by KingSun on 2017/12/16.
//

#ifndef ASSIMP_MODEL_H
#define ASSIMP_MODEL_H

#define STB_IMAGE_IMPLEMENTATION //for stb

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"

class Model{
public:
    Model(){}
    Model(const std::string &path)
    {
        open_file(path);
    }

    bool open_file(const std::string &path)
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate
                                                       | aiProcess_FlipUVs | aiProcess_GenNormals);
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cerr<<"ERROR::ASSIMP:: "<< importer.GetErrorString()<< std::endl;
            return false;
        }
        dir = path.substr(0, path.find_last_of('/')+1);
        filename = path.substr(path.find_last_of('/')+1, path.size());
//        std::cout<<dir<<"\t"<<filename<<std::endl;
        type = filename.substr(filename.find_last_of('.')+1, filename.size());

		//max_bound = to_vec3(scene->mMeshes[scene->mRootNode->mMeshes[0]]->mVertices[0]);
		//min_bound = to_vec3(scene->mMeshes[scene->mRootNode->mMeshes[0]]->mVertices[0]);

		max_bound = glm::vec3(1 < 32, 1 < 32, 1 < 32);
		min_bound = -max_bound;

        deal_node(scene->mRootNode, scene);

		center = -(max_bound + min_bound) / float(2);
		glm::vec3 c = max_bound - min_bound;
		scale = c.x > c.y ? c.x : c.y;
		//scale = scale > c.z ? scale : c.z;
		scale = 1.5 / scale;
		transform = glm::scale(transform, glm::vec3(scale, scale, scale));
		transform = glm::translate(transform, center);
    }

    bool is_valid(){ return !meshes.empty(); }

    void render(Shader shader, bool tex)
    {
        render(shader.program, tex);
    }

    void render(GLuint program, bool tex)
    {
        glUniformMatrix4fv(glGetUniformLocation(program, "u_transform"), 1, GL_FALSE, &transform[0][0]);
        for(auto &m : meshes)
        {
			if (tex) {
				m.load_textures(program);
			}
			else {
				m.disable_textures(program);
			}
            m.render(program);
        }
    }

    std::vector<Mesh> get_meshes(){ return meshes; }
    glm::vec3 get_center(){ return center; }
    GLfloat get_scale(){ return scale; }

    glm::vec3 to_vec3(aiColor3D v)
    {
        return glm::vec3(v.r, v.g, v.b);
    }

	glm::vec3 to_vec3(aiVector3D v)
	{
		return glm::vec3(v.x, v.y, v.z);
	}

    glm::vec2 to_vec2(aiVector3D v)
    {
        return glm::vec2(v.x, v.y);
    }

    glm::vec3 maximized(glm::vec3 m, glm::vec3 n)
    {
        glm::vec3 res;
        res.x = m.x > n.x ? m.x : n.x;
        res.y = m.y > n.y ? m.y : n.y;
        res.z = m.z > n.z ? m.z : n.z;
        return res;
    }

    glm::vec3 minimized(glm::vec3 m, glm::vec3 n)
    {
        glm::vec3 res;
        res.x = m.x < n.x ? m.x : n.x;
        res.y = m.y < n.y ? m.y : n.y;
        res.z = m.z < n.z ? m.z : n.z;
        return res;
    }

protected:
    void deal_node(aiNode *node, const aiScene *scene)
    {

        for(int i = 0; i < node->mNumMeshes; ++i)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            deal_mesh(mesh, scene);
        }

        for(int i = 0; i < node->mNumChildren; ++i)
        {
            deal_node(node->mChildren[i], scene);
        }
    }

    void deal_mesh(aiMesh *mesh, const aiScene *scene)
    {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        Material material;
        std::vector<Texture> textures;

        for(int i = 0; i < mesh->mNumVertices; ++i)
        {
            Vertex vertex;

            vertex.position = to_vec3(mesh->mVertices[i]);
            vertex.normal = to_vec3(mesh->mNormals[i]);

            max_bound = maximized(max_bound, vertex.position);
            min_bound = minimized(min_bound, vertex.position);

            if(mesh->HasTextureCoords(i))
            {
                glm::vec2 t;

                t.x = mesh->mTextureCoords[0][i].x;
                t.y = mesh->mTextureCoords[0][i].y;

                vertex.texcoord = to_vec2(mesh->mTextureCoords[0][i]);
            }
            vertices.emplace_back(vertex);
        }

        for(int i = 0; i < mesh->mNumFaces; ++i)
        {
            aiFace face = mesh->mFaces[i];
            for (int j = 0; j < face.mNumIndices; ++j)
            {
                indices.emplace_back(face.mIndices[j]);
            }
        }

        if(scene->HasMaterials())
        {
            aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
            material = get_material(mat);

            std::vector<Texture> tex_ambient = get_textures(mat, aiTextureType_AMBIENT);
            if(!tex_ambient.empty()) textures.insert(textures.end(), tex_ambient.begin(), tex_ambient.end());

            std::vector<Texture> tex_diffuse = get_textures(mat, aiTextureType_DIFFUSE);
            if(!tex_diffuse.empty()) textures.insert(textures.end(), tex_diffuse.begin(), tex_diffuse.end());

            std::vector<Texture> tex_specular = get_textures(mat, aiTextureType_SPECULAR);
            if(!tex_specular.empty()) textures.insert(textures.end(), tex_specular.begin(), tex_specular.end());
        }

        meshes.emplace_back(Mesh(vertices, material, textures, indices));
    }

    Material get_material(aiMaterial *mat)
    {
        Material material;
        aiColor3D color;

        mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
        if(color != aiColor3D()) material.ambient = to_vec3(color);
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		if (color != aiColor3D()) material.diffuse = to_vec3(color);
        mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
		if (color != aiColor3D()) material.specular = to_vec3(color);
        mat->Get(AI_MATKEY_SHININESS, material.shininess);
        mat->Get(AI_MATKEY_OPACITY, material.alpha);

        return material;
    }

    std::vector<Texture> get_textures(aiMaterial *mat, aiTextureType type)
    {
        std::vector<Texture> textures;

        for(int i = 0; i < mat->GetTextureCount(type); ++i)
        {
            aiString path;
            ai_real blend = 1.0; //cannot get it;
            mat->GetTexture(type, i, &path, NULL, NULL, &blend, NULL, NULL);

            bool flag = true;
            for(auto t : textures)
            {
                if(t.path == path.C_Str())
                {
					textures.emplace_back(t);
                    flag = false;
                    break;
                }
            }
            if(flag)
            {
                Texture texture;
                switch (type)
                {
                    case aiTextureType_AMBIENT:
                        texture.type = TextureType::AMBIENT;
                        break;
                    case aiTextureType_DIFFUSE:
                        texture.type = TextureType::DIFFUSE;
                        break;
                    case aiTextureType_SPECULAR:
                        texture.type = TextureType::SPECULAR;
                        break;
                }
                texture.path = path.C_Str();
                texture.blend = blend;
                texture.id = load_texture(dir+texture.path);
                textures.emplace_back(texture);
            }
        }

        return textures;
    }

    GLuint load_texture(std::string path)
    {
        GLuint textureID;
        glGenTextures(1, &textureID);

        int width, height, component;
        GLubyte *data = stbi_load(path.data(), &width, &height, &component, 0);
        if (data)
        {
            GLenum format;
            switch (component)
            {
                case 1:
                    format = GL_RED;
                    break;
                case 3:
                    format = GL_RGB;
                    break;
                case 4:
                    format = GL_RGBA;
                    break;
                default:
                    format = GL_RGB;
            }

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else
        {
            std::cerr<<"Load texture: "<<path<<" failed!"<<std::endl;
        }

        stbi_image_free(data);
        return textureID;
    }

private:
    std::vector<Mesh> meshes;

    glm::vec3 center;
    glm::vec3 max_bound, min_bound;
    GLfloat scale;
    glm::mat4 transform;

    std::string dir;
    std::string filename;
    std::string type;
};

#endif //ASSIMP_MODEL_H
