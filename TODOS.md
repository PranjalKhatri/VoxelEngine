<!-- TODO: -->
1. Change shader,texture,vertexbuffer libraries to use glm in places where i am taking values like vector as 3 floats .
2. Check out [Uniform Buffer Objects](https://wikis.khronos.org/opengl/Uniform_Buffer_Object).
3. Redesign Texture class to enable option for deleting of texture data after loading.
4. Chunk Optimizations,Voxel Optimizations.
5. Create some sort of Resouce manager abstraction for managing shaders and texture in the engine.
6. If engine stops working or crashes after initial loading, try changing the ChunkManager::Run logic first: Remove linkNeighbor, or just recreate dirty chunks or copy paste the initial load
