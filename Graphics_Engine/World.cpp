#include "World.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <unordered_set>


World::World() : m_renderDistance(2) {}

World::~World() {
    // Clean up allocated chunks
    for (auto& pair : m_chunks) {
        delete pair.second;
    }
    m_chunks.clear();
}

std::string World::GetChunkKey(int chunkX, int chunkZ) {
    return std::to_string(chunkX) + "," + std::to_string(chunkZ);
}

void World::LoadChunk(int chunkX, int chunkZ)
{
    std::string key = GetChunkKey(chunkX, chunkZ);

    if (m_chunks.find(key) == m_chunks.end()) {
        // Create a new chunk and add it to the map
        Chunk* newChunk = new Chunk(chunkX, chunkZ, this);
        m_chunks[key] = newChunk;
    }
}



void World::UnloadChunk(int chunkX, int chunkZ)
{
    std::string key = GetChunkKey(chunkX, chunkZ);
    auto it = m_chunks.find(key);
    if (it != m_chunks.end()) {
        delete it->second;
        m_chunks.erase(it);
    }
}



void World::Update(const glm::vec3& playerPosition)
{
    int currentChunkX = static_cast<int>(std::floor(playerPosition.x / static_cast<float>(Chunk::WIDTH)));
    int currentChunkZ = static_cast<int>(std::floor(playerPosition.z / static_cast<float>(Chunk::DEPTH)));

    // Set to keep track of chunks that should remain loaded
    std::unordered_set<std::string> chunksToKeep;

    // Load necessary chunks and mark them to keep
    for (int x = currentChunkX - m_renderDistance; x <= currentChunkX + m_renderDistance; ++x) {
        for (int z = currentChunkZ - m_renderDistance; z <= currentChunkZ + m_renderDistance; ++z) {
            std::string key = GetChunkKey(x, z);
            chunksToKeep.insert(key);
            LoadChunk(x, z);
        }
    }

    // Unload chunks that are outside the render distance
    for (auto it = m_chunks.begin(); it != m_chunks.end(); ) {
        if (chunksToKeep.find(it->first) == chunksToKeep.end()) {
            delete it->second;
            it = m_chunks.erase(it);
        }
        else {
            ++it;
        }
    }
}

    

void World::RenderWireframes(const Shader& shader) {
    for (auto& pair : m_chunks) {
        Chunk* chunk = pair.second;
        chunk->RenderWireframe(shader);
    }
}



void World::Render(const Shader& shader)
{
    for (auto& pair : m_chunks) {
        Chunk* chunk = pair.second;
        chunk->Render(shader);
    }
}



Chunk* World::GetChunk(int chunkX, int chunkZ) {
    std::string key = GetChunkKey(chunkX, chunkZ);
    auto it = m_chunks.find(key);
    if (it != m_chunks.end()) {
        return it->second;
    }
    return nullptr;
}

