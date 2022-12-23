#pragma once

#include <vector>

#include "shader.h"
#include "defaults.hpp"

struct Vertex {
	Vec3f Position;
	Vec3f Normal;
	Vec2f TexCoords;
};

struct PhongMaterial {
    Vec3f ambient;
    Vec3f diffuse;
    Vec3f specular;
    float shininess;
    Vec3f emission;

    PhongMaterial(float a, float b, float c, float d, float e, float f, float g, float h, float i, float j, float k, float l, float m) {
        ambient = { a, b, c };
        diffuse = { d, e, f };
        specular = { g, h, i };
        shininess = j;
        emission = { k, l, m };
    }
};


struct Light {
    Vec3f position;
    Vec3f color;
    float intensity;
    Light(float a, float b, float c, float d, float e, float f, float g) {
        position = { a, b, c };
        color = { d, e, f };
        intensity = g;
    }
};


class RenderObject {
public:
    /* Mesh Data */
    const std::vector<Vertex> vertices;
    const std::vector<unsigned int> indices;
    //std::vector<Vec3f> normal;
    unsigned int VAO, VBO, EBO;

    /* Functions */
    //MyMesh(vector<Vertex> _vertices, vector<unsigned int> _indices, vector<Texture> _textures);
    //void readMeshFile(string filename);
    RenderObject(std::vector<Vertex> &vertices_, std::vector<unsigned int> &indices_);
    RenderObject() {};
    RenderObject(const RenderObject &renderObject) = default;
    ~RenderObject();
    void Draw(Shader* shader);

    //glm::mat4 m_model_;


private:
    /* Rendering Data */
    //unsigned int VAO, VBO;// , EBO;
    void setupMesh();

};


class Model {
public:
    Model(std::shared_ptr<RenderObject> ro_,
        std::shared_ptr<Shader> shader_);

    void Draw();
    Mat44f world_transform;
    std::shared_ptr<PhongMaterial> material;
    unsigned int texture;


private:
    std::shared_ptr<RenderObject> ro;
    std::shared_ptr<Shader> shader;
    
};
