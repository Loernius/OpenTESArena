#ifndef VOXEL_TRAITS_DEFINITION_H
#define VOXEL_TRAITS_DEFINITION_H

#include "../Assets/ArenaTypes.h"

// Grab-bag traits that don't fit into other existing categories.
// @todo: eventually split this up into dedicated definitions

enum class VoxelFacing2D;

struct VoxelTraitsDefinition
{
	struct Floor
	{
		// Wild automap floor coloring to make roads, etc. easier to see.
		// @todo: maybe put in some VoxelVisibilityDefinition/VoxelAutomapTraitsDefinition?
		bool isWildWallColored;
	};

	struct Raised
	{
		double yOffset, ySize;
	};

	struct TransparentWall
	{
		// @todo: maybe put in some VoxelCollisionTraitsDefinition? For other voxels, their collision def would assume 'always a collider'.
		bool collider; // Also affects automap visibility.
	};

	struct Edge
	{
		// @todo: maybe put in some VoxelCollisionTraitsDefinition?
		VoxelFacing2D facing;
		bool collider;
	};

	struct Chasm
	{
		// @todo: should move this into LevelDefinition/LevelInfoDefinition/Chunk as a ChasmDefinition,
		// the same as DoorDefinition.
		ArenaTypes::ChasmType type;
	};

	// @todo: eventually this def should not depend on a voxel type; instead it should have things
	// like an interactivity enum (i.e. "is this a door?").
	ArenaTypes::VoxelType type;

	union
	{
		Floor floor;
		Raised raised;
		TransparentWall transparentWall;
		Edge edge;
		Chasm chasm;
	};

	VoxelTraitsDefinition();

	void initGeneral(ArenaTypes::VoxelType type); // @todo: ideally this function wouldn't be needed
	void initFloor(bool isWildWallColored);
	void initRaised(double yOffset, double ySize);
	void initTransparentWall(bool collider);
	void initEdge(VoxelFacing2D facing, bool collider);
	void initChasm(ArenaTypes::ChasmType chasmType);
};

#endif
