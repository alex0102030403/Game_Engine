#pragma once
#include <unordered_map>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <future>
#include "Chunk.h"

#include "ThreadPool.h"

class Chunk;
class Shader;



class World {
public:
    World();
    ~World();

    void Update(const glm::vec3& playerPosition);
    void Render(const Shader& shader);
    
    void RenderWireframes(const Shader& shader);

    Chunk* GetChunk(int chunkX, int chunkZ);


private:
    std::unordered_map<std::string, Chunk*> m_chunks;
    

    // Helper functions
    std::string GetChunkKey(int chunkX, int chunkZ);
    void LoadChunk(int chunkX, int chunkZ);
    void UnloadChunk(int chunkX, int chunkZ);
    
    int m_renderDistance; // Number of chunks to load around the player
};
