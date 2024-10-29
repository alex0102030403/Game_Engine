#include "Chunk.h"
#include <iostream>
#include <gtc/matrix_transform.hpp>
#include "PerlinNoise.h" // Include a Perlin noise implementation
#include "World.h"

Chunk::Chunk(int chunkX, int chunkZ, World* world)
    : m_chunkX(chunkX), m_chunkZ(chunkZ), world(world)
{
    // Calculate the chunk's world position
    m_worldPosition = glm::vec3(
        m_chunkX * WIDTH,
        0.0f,
        m_chunkZ * DEPTH
    );

    GenerateTerrain();
    GenerateMesh();

    // Create and fill the buffer
    m_buffer.CreateBuffer(m_indices.size(), true);
    m_buffer.FillEBO(m_indices.data(), m_indices.size() * sizeof(unsigned int), Buffer::FillType::Once);
    m_buffer.FillVBO(Buffer::VBOType::VertexBuffer, m_vertices.data(), m_vertices.size() * sizeof(float), Buffer::FillType::Once);
    m_buffer.FillVBO(Buffer::VBOType::ColorBuffer, m_colors.data(), m_colors.size() * sizeof(float), Buffer::FillType::Once);
    m_buffer.FillVBO(Buffer::VBOType::NormalBuffer, m_normals.data(), m_normals.size() * sizeof(float), Buffer::FillType::Once);
    m_buffer.LinkEBO();
}


Chunk::~Chunk() {
    m_buffer.DestroyBuffer();
}

void Chunk::CreateBuffers() {
    // OpenGL buffer creation and data upload
    m_buffer.CreateBuffer(m_indices.size(), true);
    m_buffer.FillEBO(m_indices.data(), m_indices.size() * sizeof(unsigned int), Buffer::FillType::Once);
    m_buffer.FillVBO(Buffer::VBOType::VertexBuffer, m_vertices.data(), m_vertices.size() * sizeof(float), Buffer::FillType::Once);
    m_buffer.FillVBO(Buffer::VBOType::ColorBuffer, m_colors.data(), m_colors.size() * sizeof(float), Buffer::FillType::Once);
    m_buffer.FillVBO(Buffer::VBOType::NormalBuffer, m_normals.data(), m_normals.size() * sizeof(float), Buffer::FillType::Once);
    m_buffer.LinkEBO();
}

void Chunk::GenerateTerrain()
{
    // Terrain generation using Perlin noise
    PerlinNoise noise;
    double scale = 0.1;

    for (int x = 0; x < WIDTH; ++x) {
        for (int z = 0; z < DEPTH; ++z) {
            int worldX = x + m_chunkX * WIDTH;
            int worldZ = z + m_chunkZ * DEPTH;

            double nx = worldX * scale;
            double nz = worldZ * scale;
            double elevation = noise.noise(nx, nz);

            int h = static_cast<int>(elevation * (HEIGHT - 1));

            for (int y = 0; y <= h; ++y) {
                Voxel::Type type = Voxel::Dirt;
                if (y == h) {
                    type = Voxel::Grass;
                }
                else if (y < h - 5) {
                    type = Voxel::Stone;
                }
                m_voxels[x][y][z].SetType(type);
            }
        }
    }
}


void Chunk::GenerateMesh() {
    m_vertices.clear();
    m_normals.clear();
    m_colors.clear();
    m_indices.clear();

    unsigned int index = 0;

    // Offsets for cube faces
    const glm::vec3 faceOffsets[6] = {
        glm::vec3(0, 0, 1),  // Front
        glm::vec3(0, 0, -1), // Back
        glm::vec3(-1, 0, 0), // Left
        glm::vec3(1, 0, 0),  // Right
        glm::vec3(0, 1, 0),  // Top
        glm::vec3(0, -1, 0)  // Bottom
    };

    const glm::vec3 faceNormals[6] = {
        glm::vec3(0, 0, 1),  // Front
        glm::vec3(0, 0, -1), // Back
        glm::vec3(-1, 0, 0), // Left
        glm::vec3(1, 0, 0),  // Right
        glm::vec3(0, 1, 0),  // Top
        glm::vec3(0, -1, 0)  // Bottom
    };

    // Indices for a face (two triangles)
    const unsigned int faceIndices[] = {
        0, 1, 2, 2, 3, 0
    };

    // Relative positions of vertices for a face
    const glm::vec3 faceVertices[6][4] = {
        { // Front face
            glm::vec3(0, 0, 1),
            glm::vec3(1, 0, 1),
            glm::vec3(1, 1, 1),
            glm::vec3(0, 1, 1)
        },
        { // Back face
            glm::vec3(1, 0, 0),
            glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0),
            glm::vec3(1, 1, 0)
        },
        { // Left face
            glm::vec3(0, 0, 0),
            glm::vec3(0, 0, 1),
            glm::vec3(0, 1, 1),
            glm::vec3(0, 1, 0)
        },
        { // Right face
            glm::vec3(1, 0, 1),
            glm::vec3(1, 0, 0),
            glm::vec3(1, 1, 0),
            glm::vec3(1, 1, 1)
        },
        { // Top face
            glm::vec3(0, 1, 1),
            glm::vec3(1, 1, 1),
            glm::vec3(1, 1, 0),
            glm::vec3(0, 1, 0)
        },
        { // Bottom face
            glm::vec3(0, 0, 0),
            glm::vec3(1, 0, 0),
            glm::vec3(1, 0, 1),
            glm::vec3(0, 0, 1)
        }
    };

    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            for (int z = 0; z < DEPTH; ++z) {
                if (m_voxels[x][y][z].IsActive()) {
                    // Get the voxel color
                    glm::vec3 color = m_voxels[x][y][z].GetColor();

                    // For each face
                    for (int face = 0; face < 6; ++face) {
                        int nx = x + (int)faceOffsets[face].x;
                        int ny = y + (int)faceOffsets[face].y;
                        int nz = z + (int)faceOffsets[face].z;

                        if (!IsVoxelActive(nx, ny, nz)) {
                            // Add face
                            // Add vertices
                            for (int i = 0; i < 4; ++i) {
                                // Adjust the position by adding the chunk's world position
                                glm::vec3 position = m_worldPosition + glm::vec3(x, y, z) + faceVertices[face][i];
                                m_vertices.push_back(position.x);
                                m_vertices.push_back(position.y);
                                m_vertices.push_back(position.z);

                                // Normals
                                m_normals.push_back(faceNormals[face].x);
                                m_normals.push_back(faceNormals[face].y);
                                m_normals.push_back(faceNormals[face].z);

                                // Colors
                                m_colors.push_back(color.r);
                                m_colors.push_back(color.g);
                                m_colors.push_back(color.b);
                            }

                            // Add indices
                            m_indices.push_back(index + faceIndices[0]);
                            m_indices.push_back(index + faceIndices[1]);
                            m_indices.push_back(index + faceIndices[2]);
                            m_indices.push_back(index + faceIndices[3]);
                            m_indices.push_back(index + faceIndices[4]);
                            m_indices.push_back(index + faceIndices[5]);

                            index += 4; // Move to next set of vertices
                        }
                    }
                }
            }
        }
    }
}

bool Chunk::IsVoxelActive(int x, int y, int z) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT || z < 0 || z >= DEPTH) {
        // Check neighboring chunks
        int neighborChunkX = m_chunkX;
        int neighborChunkZ = m_chunkZ;
        int neighborX = x;
        int neighborY = y;
        int neighborZ = z;

        if (x < 0) {
            neighborChunkX = m_chunkX - 1;
            neighborX = x + WIDTH;
        }
        else if (x >= WIDTH) {
            neighborChunkX = m_chunkX + 1;
            neighborX = x - WIDTH;
        }

        if (z < 0) {
            neighborChunkZ = m_chunkZ - 1;
            neighborZ = z + DEPTH;
        }
        else if (z >= DEPTH) {
            neighborChunkZ = m_chunkZ + 1;
            neighborZ = z - DEPTH;
        }

        // Get the neighboring chunk
        
        Chunk* neighborChunk = world->GetChunk(neighborChunkX, neighborChunkZ);
        if (neighborChunk) {
            return neighborChunk->IsVoxelActive(neighborX, neighborY, neighborZ);
        }
        else {
            // If the neighbor chunk isn't loaded, assume it's empty
            return false;
        }
    }
    return m_voxels[x][y][z].IsActive();
}


void Chunk::Update() {
    // Implement any updates if necessary
}

void Chunk::Render(const Shader& shader)
{
    // Set model matrix
    glm::mat4 model = glm::translate(glm::mat4(1.0f), m_worldPosition);
    shader.SendUniformData("model", model);

    // Set material properties
    shader.SendUniformData("isLit", true);
    shader.SendUniformData("isTextured", false); // No textures

    // Link buffers
    m_buffer.LinkVBO(shader, "vertexIn", Buffer::VBOType::VertexBuffer, Buffer::ComponentType::XYZ, Buffer::DataType::FloatData);
    m_buffer.LinkVBO(shader, "colorIn", Buffer::VBOType::ColorBuffer, Buffer::ComponentType::RGB, Buffer::DataType::FloatData);
    m_buffer.LinkVBO(shader, "normalIn", Buffer::VBOType::NormalBuffer, Buffer::ComponentType::XYZ, Buffer::DataType::FloatData);

    m_buffer.Render(Buffer::DrawType::Triangles);
}

void Chunk::InitializeWireframe() {
    if (m_wireframeVAO != 0) return; // Already initialized

    // Vertices of the cube, adjusted to chunk's local coordinates
    GLfloat vertices[] = {
        0.0f, 0.0f, 0.0f,                        // 0
        (float)WIDTH, 0.0f, 0.0f,                // 1
        (float)WIDTH, 0.0f, (float)DEPTH,        // 2
        0.0f, 0.0f, (float)DEPTH,                // 3
        0.0f, (float)HEIGHT, 0.0f,               // 4
        (float)WIDTH, (float)HEIGHT, 0.0f,       // 5
        (float)WIDTH, (float)HEIGHT, (float)DEPTH,//6
        0.0f, (float)HEIGHT, (float)DEPTH        // 7
    };

    GLuint indices[] = {
        // Bottom face edges
        0, 1, 1, 2, 2, 3, 3, 0,
        // Top face edges
        4, 5, 5, 6, 6, 7, 7, 4,
        // Vertical edges
        0, 4, 1, 5, 2, 6, 3, 7
    };

    glGenVertexArrays(1, &m_wireframeVAO);
    glGenBuffers(1, &m_wireframeVBO);
    glGenBuffers(1, &m_wireframeEBO);

    glBindVertexArray(m_wireframeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_wireframeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_wireframeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // Unbind VAO
}

void Chunk::RenderWireframe(const Shader& shader) {
    InitializeWireframe(); // Ensure the wireframe mesh is initialized

    // Set the model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_worldPosition);

    shader.SendUniformData("model", model);

    // Set color uniform (assuming your shader has a uniform for color)
    shader.SendUniformData("color", 1.0f, 1.0f, 1.0f); // White color

    // Disable face culling and depth testing if needed
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // Draw lines
    glBindVertexArray(m_wireframeVAO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


