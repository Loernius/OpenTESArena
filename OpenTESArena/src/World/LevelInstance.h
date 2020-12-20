#ifndef LEVEL_INSTANCE_H
#define LEVEL_INSTANCE_H

#include "ChunkManager.h"
#include "../Entities/EntityManager.h"

// Instance of a level with voxels and entities. Its data is in a baked, context-sensitive format
// and depends on one or more level definitions for its population.

class MapDefinition;

enum class WorldType;

class LevelInstance
{
private:
	ChunkManager chunkManager;
	EntityManager entityManager;
public:
	void init();

	ChunkManager &getChunkManager();
	const ChunkManager &getChunkManager() const;
	EntityManager &getEntityManager();
	const EntityManager &getEntityManager() const;

	void update(double dt, const ChunkInt2 &centerChunk, const MapDefinition &mapDefinition,
		int chunkDistance);
};

#endif
