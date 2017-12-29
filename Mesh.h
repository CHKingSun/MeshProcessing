//
// Created by KingSun on 2017/12/16.
//

#ifndef ASSIMP_MESH_H
#define ASSIMP_MESH_H

#include <string>
#include <vector>
#include <vec3.hpp>
#include <vec2.hpp>
#include <vec4.hpp>

#include <GL/glew.h>
#include <GL/freeglut.h>

struct Vertex{
    Vertex(){
        position = glm::vec3(0.0, 0.0, 0.0);
        normal = glm::vec3(0.0, 0.0, 0.0);
        texcoord = glm::vec2(0.0, 0.0);
    }
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
};

struct Material{
    Material(){
        ambient = glm::vec3(0.2, 0.2, 0.2);
        diffuse = glm::vec3(0.8, 0.8, 0.8);
        specular = glm::vec3(1.0, 1.0, 1.0);
        shininess = 0;
        alpha = 1;
    }
    glm::vec3 ambient, diffuse, specular;
    GLfloat shininess;
    GLfloat alpha;
};

enum TextureType{
    AMBIENT, DIFFUSE, SPECULAR
};

struct Texture{
    GLuint id;
    GLfloat blend;
    std::string path;
    TextureType type;
};

class Mesh{
public:
    Mesh(){}
    Mesh(std::vector<Vertex> v, Material m, std::vector<Texture> t, std::vector<GLuint> i)
    {
        vertices = v;
        material = m;
        textures = t;
        indices = i;

        load_vertices();
    }

    void set_vertices(std::vector<Vertex> vertices){ this->vertices = vertices; }
    void set_materials(Material materials){ this->material = material; }
    void set_textures(std::vector<Texture> textures){ this->textures = textures; }
    void set_indices(std::vector<GLuint> indices){ this->indices = indices; }

    Vertex *get_vertices(){ return vertices.data(); }
    Material get_materials(){ return material; }
    Texture *get_textures(){ return textures.data(); }
    GLuint *get_indices(){ return indices.data(); }

    unsigned int n_vertices(){ return vertices.size(); }
    int n_materials(){ return vertices.size(); }
    int n_textures(){ return textures.size(); }
    unsigned int n_indices(){ return indices.size(); }

    void render(GLuint program)
    {
		//GLuint program = shader.program;

        load_material(program);
        //load_textures(program);

        GLuint a_position = glGetAttribLocation(program, "a_position");
        GLuint a_normal = glGetAttribLocation(program, "a_normal");

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glEnableVertexAttribArray(a_position);
        glVertexAttribPointer(a_position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(a_normal);
        glVertexAttribPointer(a_normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
       
		//std::cout<<sizeof(Vertex)<<"\t" << offsetof(Vertex, position) << "\t" 
		//	<< offsetof(Vertex, normal) << "\t"<< offsetof(Vertex, texcoord) << std::endl;

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glDisableVertexAttribArray(a_position);
        glDisableVertexAttribArray(a_normal);
        glDisableVertexAttribArray(glGetAttribLocation(program, "a_texcoord"));
    }

	void load_textures(GLuint program)
	{
		GLuint a_texcoord = glGetAttribLocation(program, "a_texcoord");
		glEnableVertexAttribArray(a_texcoord);
		glVertexAttribPointer(a_texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));

		auto it = textures.begin();
		for (int i = 0; i < textures.size(); ++i, ++it)
		{
			std::string u_textures = "u_textures[" + std::to_string(i) + "]";
			glUniform1i(glGetUniformLocation(program, (u_textures + ".enable").data()), true);
			glUniform1f(glGetUniformLocation(program, (u_textures + ".blend").data()), (*it).blend);
			glUniform1i(glGetUniformLocation(program, (u_textures + ".type").data()), (*it).type);

			glActiveTexture(GL_TEXTURE0+i);
			glUniform1i(glGetUniformLocation(program, (u_textures + ".texture").data()), i);
            glBindTexture(GL_TEXTURE_2D, (*it).id);
		}
	}

	void disable_textures(GLuint program) 
	{
		auto it = textures.begin();
		for (int i = 0; i < textures.size(); ++i, ++it)
		{
			glUniform1i(glGetUniformLocation(program, ("u_textures[" + std::to_string(i) + "].enable").data()), false);
		}
	}

protected:
    void load_vertices()
    {
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
    }

    void load_material(GLuint program)
    {
        glUniform3fv(glGetUniformLocation(program, "u_ambient"), 1, &material.ambient[0]);
        glUniform3fv(glGetUniformLocation(program, "u_diffuse"), 1, &material.diffuse[0]);
        glUniform3fv(glGetUniformLocation(program, "u_specular"), 1, &material.specular[0]);
        glUniform1f(glGetUniformLocation(program, "u_shininess"), material.shininess);
        glUniform1f(glGetUniformLocation(program, "u_alpha"), material.alpha);
    }

private:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    Material material;
    std::vector<Texture> textures;

    GLuint VBO, IBO;
};

#endif //ASSIMP_MESH_H
