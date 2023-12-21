#include "include/model.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
        : vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures)), VAO(0), VBO(0),
          EBO(0)
{
    setupMesh();
}

void Mesh::draw(Shader &shader)
{
    GLuint diffuseNr = 1, specularNr = 1;
    for (GLuint i = 0; i < textures.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = textures[i].type;

        if (name == "texture_diffuse") number = std::to_string(diffuseNr++);
        else if (name == "texture_specular") number = std::to_string(specularNr++);

        std::string uniformName = "material.";
        uniformName += name;
        uniformName += number;

        shader.setInt(uniformName, static_cast<GLint>(i));
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLint>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    std::vector<GLfloat> meshVertices;
    meshVertices.reserve(vertices.size() * 8);
    for (const auto &vertex: vertices)
    {
        meshVertices.push_back(vertex.position.x);
        meshVertices.push_back(vertex.position.y);
        meshVertices.push_back(vertex.position.z);

        meshVertices.push_back(vertex.normal.x);
        meshVertices.push_back(vertex.normal.y);
        meshVertices.push_back(vertex.normal.z);

        meshVertices.push_back(vertex.texCoords.x);
        meshVertices.push_back(vertex.texCoords.y);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<long>(meshVertices.size() * sizeof(GLfloat)), meshVertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long>(indices.size() * sizeof(GLuint)), indices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

Model::Model(const GLchar* path) { loadModel(path); }
void Model::draw(Shader &shader) { for (auto mesh: meshes) mesh.draw(shader); }

void Model::loadModel(const std::string &path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for (GLuint i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (GLuint i = 0; i < node->mNumChildren; ++i) processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    vertices.reserve(mesh->mNumVertices);
    for (GLuint i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex = {};

        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0])
        {
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        } else vertex.texCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    indices.reserve(mesh->mNumFaces * 3);
    for (GLuint i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (GLuint j = 0; j < face.mNumIndices; ++j) indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return {vertices, indices, textures};
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string &typeName)
{
    std::vector<Texture> textures;

    for (GLuint i = 0; i < mat->GetTextureCount(type); ++i)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        GLboolean skip = false;
        for (const auto &texture: texturesLoaded)
        {
            if (std::strcmp(texture.path.data(), str.C_Str()) == 0)
            {
                textures.push_back(texture);
                skip = true;

                break;
            }
        }

        if (!skip)
        {
            Texture texture = {};
            texture.id = loadTextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();

            textures.push_back(texture);
            texturesLoaded.push_back(texture);
        }
    }

    return textures;
}

GLuint Model::loadTextureFromFile(const GLchar* path, const std::string &dir)
{
    std::string filename = std::string(path);
    filename = dir + '/' + filename;

    GLuint textureID;
    glGenTextures(1, &textureID);

    GLint width, height, channels;
    GLubyte* image = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if (image)
    {
        GLint format = 0;
        if (channels == 1) format = GL_RED;
        else if (channels == 3) format = GL_RGB;
        else if (channels == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, format == GL_RGBA ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, format == GL_RGBA ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR);

        stbi_image_free(image);
    } else
    {
        std::cerr << "Texture failed to load from path: " << path << std::endl;
        stbi_image_free(image);
    }

    return textureID;
}
