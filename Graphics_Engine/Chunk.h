#pragma once
#include <vector>
#include <glm.hpp>
#include "World.h"
#include "Voxel.h"
#include "Shader.h"
#include "Buffer.h"


class World;

class Chunk {
public:
    static const int WIDTH = 16;
    static const int HEIGHT = 32;
    static const int DEPTH = 16;

    Chunk(int chunkX, int chunkZ, World* world);
    ~Chunk();

    void GenerateTerrain();
    void Update();
    void Render(const Shader& shader);

    void RenderWireframe(const Shader& shader);

    void CreateBuffers();

	int GetChunkX() const { return m_chunkX; }
	int GetChunkZ() const { return m_chunkZ; }

    void GenerateMesh();
    bool IsVoxelActive(int x, int y, int z);


private:

    World* world;
    int m_chunkX, m_chunkZ; // Chunk position in the world
    glm::vec3 m_worldPosition;


    Voxel m_voxels[WIDTH][HEIGHT][DEPTH];

    // Mesh data
    std::vector<float> m_vertices;
    std::vector<float> m_normals;
    std::vector<float> m_colors;    // Add color data
    std::vector<unsigned int> m_indices;

    GLuint m_wireframeVAO = 0;
    GLuint m_wireframeVBO = 0;
    GLuint m_wireframeEBO = 0;

    void InitializeWireframe();

    Buffer m_buffer;



    
};
