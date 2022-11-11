#ifndef ARENA_MESH_UTILS_H
#define ARENA_MESH_UTILS_H

#include <array>
#include <cstdint>

#include "MeshUtils.h"
#include "../Assets/ArenaTypes.h"

#include "components/debug/Debug.h"
#include "components/utilities/BufferView.h"

// Note that the original game doesn't actually have meshes - this is just a convenient way to define things.

enum class VoxelFacing2D;

namespace ArenaMeshUtils
{
	static constexpr int MAX_VERTICES = 24;
	static constexpr int MAX_INDICES = 36;

	using ChasmWallIndexBuffer = std::array<int32_t, 6>; // Two triangles per buffer.

	struct InitCache
	{
		std::array<double, MAX_VERTICES * MeshUtils::POSITION_COMPONENTS_PER_VERTEX> vertices;
		std::array<double, MAX_VERTICES * MeshUtils::NORMAL_COMPONENTS_PER_VERTEX> normals;
		std::array<double, MAX_VERTICES * MeshUtils::TEX_COORDS_PER_VERTEX> texCoords;
		std::array<int32_t, MAX_INDICES> opaqueIndices0, opaqueIndices1, opaqueIndices2;
		std::array<int32_t, MAX_INDICES> alphaTestedIndices0;
		std::array<const decltype(opaqueIndices0)*, 3> opaqueIndicesPtrs;

		BufferView<double> verticesView, normalsView, texCoordsView;
		BufferView<int32_t> opaqueIndices0View, opaqueIndices1View, opaqueIndices2View, alphaTestedIndices0View;

		InitCache()
		{
			this->vertices.fill(0.0);
			this->normals.fill(0.0);
			this->texCoords.fill(0.0);
			this->opaqueIndices0.fill(-1);
			this->opaqueIndices1.fill(-1);
			this->opaqueIndices2.fill(-1);
			this->alphaTestedIndices0.fill(-1);
			this->opaqueIndicesPtrs = { &this->opaqueIndices0, &this->opaqueIndices1, &this->opaqueIndices2 };

			this->verticesView = BufferView<double>(this->vertices.data(), static_cast<int>(this->vertices.size()));
			this->normalsView = BufferView<double>(this->normals.data(), static_cast<int>(this->normals.size()));
			this->texCoordsView = BufferView<double>(this->texCoords.data(), static_cast<int>(this->texCoords.size()));
			this->opaqueIndices0View = BufferView<int32_t>(this->opaqueIndices0.data(), static_cast<int>(this->opaqueIndices0.size()));
			this->opaqueIndices1View = BufferView<int32_t>(this->opaqueIndices1.data(), static_cast<int>(this->opaqueIndices1.size()));
			this->opaqueIndices2View = BufferView<int32_t>(this->opaqueIndices2.data(), static_cast<int>(this->opaqueIndices2.size()));
			this->alphaTestedIndices0View = BufferView<int32_t>(this->alphaTestedIndices0.data(), static_cast<int>(this->alphaTestedIndices0.size()));
		}
	};

	// The "ideal" vertices per voxel (no duplication).
	constexpr int GetUniqueVertexCount(ArenaTypes::VoxelType voxelType)
	{
		switch (voxelType)
		{
		case ArenaTypes::VoxelType::None:
			return 0;
		case ArenaTypes::VoxelType::Wall:
		case ArenaTypes::VoxelType::Raised:
		case ArenaTypes::VoxelType::TransparentWall:
		case ArenaTypes::VoxelType::Chasm:
		case ArenaTypes::VoxelType::Door:
			return 8;
		case ArenaTypes::VoxelType::Floor:
		case ArenaTypes::VoxelType::Ceiling:
		case ArenaTypes::VoxelType::Diagonal:
		case ArenaTypes::VoxelType::Edge:
			return 4;
		default:
			DebugUnhandledReturnMsg(int, std::to_string(static_cast<int>(voxelType)));
		}
	}

	// The actual vertices per voxel used by the renderer due to how vertex attributes work.
	constexpr int GetRendererVertexCount(ArenaTypes::VoxelType voxelType)
	{
		switch (voxelType)
		{
		case ArenaTypes::VoxelType::None:
			return 0;
		case ArenaTypes::VoxelType::Wall:
		case ArenaTypes::VoxelType::Raised:
			return 24;
		case ArenaTypes::VoxelType::TransparentWall:
		case ArenaTypes::VoxelType::Door:
			return 16;
		case ArenaTypes::VoxelType::Chasm:
			return 20;
		case ArenaTypes::VoxelType::Floor:
		case ArenaTypes::VoxelType::Ceiling:
		case ArenaTypes::VoxelType::Diagonal:
		case ArenaTypes::VoxelType::Edge:
			return 4;
		default:
			DebugUnhandledReturnMsg(int, std::to_string(static_cast<int>(voxelType)));
		}
	}

	constexpr int GetRendererVertexPositionComponentCount(ArenaTypes::VoxelType voxelType)
	{
		return GetRendererVertexCount(voxelType) * MeshUtils::POSITION_COMPONENTS_PER_VERTEX;
	}

	constexpr int GetRendererVertexNormalComponentCount(ArenaTypes::VoxelType voxelType)
	{
		return GetRendererVertexCount(voxelType) * MeshUtils::NORMAL_COMPONENTS_PER_VERTEX;
	}

	constexpr int GetRendererVertexTexCoordCount(ArenaTypes::VoxelType voxelType)
	{
		return GetRendererVertexCount(voxelType) * MeshUtils::TEX_COORDS_PER_VERTEX;
	}

	constexpr int GetOpaqueIndexBufferCount(ArenaTypes::VoxelType voxelType)
	{
		switch (voxelType)
		{
		case ArenaTypes::VoxelType::Wall:
			return 3;
		case ArenaTypes::VoxelType::Raised:
			return 2;
		case ArenaTypes::VoxelType::Floor:
		case ArenaTypes::VoxelType::Ceiling:
		case ArenaTypes::VoxelType::Diagonal:
		case ArenaTypes::VoxelType::Chasm:
			return 1;
		case ArenaTypes::VoxelType::None:
		case ArenaTypes::VoxelType::TransparentWall:
		case ArenaTypes::VoxelType::Edge:
		case ArenaTypes::VoxelType::Door:
			return 0;
		default:
			DebugUnhandledReturnMsg(int, std::to_string(static_cast<int>(voxelType)));
		}
	}

	constexpr int GetOpaqueIndexCount(ArenaTypes::VoxelType voxelType, int bufferIndex)
	{
		int triangleCount = -1;

		switch (voxelType)
		{
		case ArenaTypes::VoxelType::None:
		case ArenaTypes::VoxelType::TransparentWall:
		case ArenaTypes::VoxelType::Door:
		case ArenaTypes::VoxelType::Edge:
			// @todo: should this static_assert false instead?
			DebugUnhandledReturnMsg(int, std::to_string(static_cast<int>(voxelType)) + " " + std::to_string(bufferIndex));
		case ArenaTypes::VoxelType::Wall:
			if (bufferIndex == 0)
			{
				triangleCount = 8;
			}
			else if ((bufferIndex == 1) || (bufferIndex == 2))
			{
				triangleCount = 2;
			}
			else
			{
				DebugUnhandledReturnMsg(int, std::to_string(static_cast<int>(voxelType)) + " " + std::to_string(bufferIndex));
			}
			break;
		case ArenaTypes::VoxelType::Raised:
			if ((bufferIndex == 0) || (bufferIndex == 1))
			{
				triangleCount = 4;
			}
			else
			{
				DebugUnhandledReturnMsg(int, std::to_string(static_cast<int>(voxelType)) + " " + std::to_string(bufferIndex));
			}
			break;
		case ArenaTypes::VoxelType::Chasm:
		case ArenaTypes::VoxelType::Floor:
		case ArenaTypes::VoxelType::Ceiling:
		case ArenaTypes::VoxelType::Diagonal:
			if (bufferIndex == 0)
			{
				triangleCount = 2;
			}
			else
			{
				DebugUnhandledReturnMsg(int, std::to_string(static_cast<int>(voxelType)) + " " + std::to_string(bufferIndex));
			}
			break;
		default:
			DebugUnhandledReturnMsg(int, std::to_string(static_cast<int>(voxelType)));
		}

		return triangleCount * MeshUtils::INDICES_PER_TRIANGLE;
	}

	constexpr int GetAlphaTestedIndexBufferCount(ArenaTypes::VoxelType voxelType)
	{
		switch (voxelType)
		{
		case ArenaTypes::VoxelType::None:
		case ArenaTypes::VoxelType::Wall:
		case ArenaTypes::VoxelType::Floor:
		case ArenaTypes::VoxelType::Ceiling:
		case ArenaTypes::VoxelType::Diagonal:
		case ArenaTypes::VoxelType::Chasm:
			return 0;
		case ArenaTypes::VoxelType::Raised:
		case ArenaTypes::VoxelType::TransparentWall:
		case ArenaTypes::VoxelType::Edge:
		case ArenaTypes::VoxelType::Door:
			return 1;
		default:
			DebugUnhandledReturnMsg(int, std::to_string(static_cast<int>(voxelType)));
		}
	}

	constexpr int GetAlphaTestedIndexCount(ArenaTypes::VoxelType voxelType, int bufferIndex)
	{
		int triangleCount = -1;

		switch (voxelType)
		{
		case ArenaTypes::VoxelType::None:
		case ArenaTypes::VoxelType::Wall:
		case ArenaTypes::VoxelType::Floor:
		case ArenaTypes::VoxelType::Ceiling:
		case ArenaTypes::VoxelType::Diagonal:
		case ArenaTypes::VoxelType::Chasm:
			DebugUnhandledReturnMsg(int, std::to_string(static_cast<int>(voxelType)) + " " + std::to_string(bufferIndex));
		case ArenaTypes::VoxelType::Raised:
			if (bufferIndex == 0)
			{
				triangleCount = 12;
			}
			else
			{
				DebugUnhandledReturnMsg(int, std::to_string(static_cast<int>(voxelType)) + " " + std::to_string(bufferIndex));
			}
			break;
		case ArenaTypes::VoxelType::TransparentWall:
		case ArenaTypes::VoxelType::Door:
			if (bufferIndex == 0)
			{
				triangleCount = 8;
			}
			else
			{
				DebugUnhandledReturnMsg(int, std::to_string(static_cast<int>(voxelType)) + " " + std::to_string(bufferIndex));
			}
			break;
		case ArenaTypes::VoxelType::Edge:
			if (bufferIndex == 0)
			{
				triangleCount = 2;
			}
			else
			{
				DebugUnhandledReturnMsg(int, std::to_string(static_cast<int>(voxelType)) + " " + std::to_string(bufferIndex));
			}
			break;
		default:
			DebugUnhandledReturnMsg(int, std::to_string(static_cast<int>(voxelType)));
		}

		return triangleCount * MeshUtils::INDICES_PER_TRIANGLE;
	}

	constexpr bool AllowsBackFacingGeometry(ArenaTypes::VoxelType voxelType)
	{
		switch (voxelType)
		{
		case ArenaTypes::VoxelType::None:
		case ArenaTypes::VoxelType::Wall:
		case ArenaTypes::VoxelType::Floor:
		case ArenaTypes::VoxelType::Ceiling:
		case ArenaTypes::VoxelType::Raised:
		case ArenaTypes::VoxelType::TransparentWall:
		case ArenaTypes::VoxelType::Door:
			return false;
		case ArenaTypes::VoxelType::Diagonal:
		case ArenaTypes::VoxelType::Edge:
		case ArenaTypes::VoxelType::Chasm:
			return true;
		default:
			DebugUnhandledReturnMsg(bool, std::to_string(static_cast<int>(voxelType)));
		}
	}

	constexpr bool EnablesNeighborVoxelGeometry(ArenaTypes::VoxelType voxelType)
	{
		switch (voxelType)
		{
		case ArenaTypes::VoxelType::None:
		case ArenaTypes::VoxelType::Chasm:
			return false;
		case ArenaTypes::VoxelType::Wall:
		case ArenaTypes::VoxelType::Floor:
		case ArenaTypes::VoxelType::Ceiling:
		case ArenaTypes::VoxelType::Raised:
		case ArenaTypes::VoxelType::Diagonal:
		case ArenaTypes::VoxelType::TransparentWall:
		case ArenaTypes::VoxelType::Edge:
		case ArenaTypes::VoxelType::Door:
			return true;
		default:
			DebugUnhandledReturnMsg(bool, std::to_string(static_cast<int>(voxelType)));
		}
	}

	constexpr bool HasContextSensitiveGeometry(ArenaTypes::VoxelType voxelType)
	{
		switch (voxelType)
		{
		case ArenaTypes::VoxelType::None:
		case ArenaTypes::VoxelType::Wall:
		case ArenaTypes::VoxelType::Floor:
		case ArenaTypes::VoxelType::Ceiling:
		case ArenaTypes::VoxelType::Raised:
		case ArenaTypes::VoxelType::Diagonal:
		case ArenaTypes::VoxelType::TransparentWall:
		case ArenaTypes::VoxelType::Edge:
		case ArenaTypes::VoxelType::Door:
			return false;
		case ArenaTypes::VoxelType::Chasm:
			return true;
		default:
			DebugUnhandledReturnMsg(bool, std::to_string(static_cast<int>(voxelType)));
		}
	}

	// Mesh writing functions. All of these are in unscaled model space.
	void WriteWallMeshGeometryBuffers(BufferView<double> outVertices, BufferView<double> outNormals, BufferView<double> outTexCoords);
	void WriteWallMeshIndexBuffers(BufferView<int32_t> outOpaqueSideIndices, BufferView<int32_t> outOpaqueBottomIndices, BufferView<int32_t> outOpaqueTopIndices);
	void WriteFloorMeshGeometryBuffers(BufferView<double> outVertices, BufferView<double> outNormals, BufferView<double> outTexCoords);
	void WriteFloorMeshIndexBuffers(BufferView<int32_t> outOpaqueIndices);
	void WriteCeilingMeshGeometryBuffers(BufferView<double> outVertices, BufferView<double> outNormals, BufferView<double> outTexCoords);
	void WriteCeilingMeshIndexBuffers(BufferView<int32_t> outOpaqueIndices);
	void WriteRaisedMeshGeometryBuffers(double yOffset, double ySize, double vBottom, double vTop, BufferView<double> outVertices, BufferView<double> outNormals, BufferView<double> outTexCoords);
	void WriteRaisedMeshIndexBuffers(BufferView<int32_t> outAlphaTestedSideIndices, BufferView<int32_t> outOpaqueBottomIndices, BufferView<int32_t> outOpaqueTopIndices);
	void WriteDiagonalMeshGeometryBuffers(bool type1, BufferView<double> outVertices, BufferView<double> outNormals, BufferView<double> outTexCoords);
	void WriteDiagonalMeshIndexBuffers(BufferView<int32_t> outOpaqueIndices);
	void WriteTransparentWallMeshGeometryBuffers(BufferView<double> outVertices, BufferView<double> outNormals, BufferView<double> outTexCoords);
	void WriteTransparentWallMeshIndexBuffers(BufferView<int32_t> outAlphaTestedIndices);
	void WriteEdgeMeshGeometryBuffers(VoxelFacing2D facing, double yOffset, bool flipped, BufferView<double> outVertices, BufferView<double> outNormals, BufferView<double> outTexCoords);
	void WriteEdgeMeshIndexBuffers(BufferView<int32_t> outAlphaTestedIndices);
	void WriteChasmMeshGeometryBuffers(ArenaTypes::ChasmType chasmType, BufferView<double> outVertices, BufferView<double> outNormals, BufferView<double> outTexCoords);
	void WriteChasmFloorMeshIndexBuffers(BufferView<int32_t> outOpaqueIndices); // Chasm walls are separate because they're conditionally enabled.
	void WriteChasmWallMeshIndexBuffers(ChasmWallIndexBuffer *outNorthIndices, ChasmWallIndexBuffer *outEastIndices, ChasmWallIndexBuffer *outSouthIndices, ChasmWallIndexBuffer *outWestIndices);
	void WriteDoorMeshGeometryBuffers(BufferView<double> outVertices, BufferView<double> outNormals, BufferView<double> outTexCoords);
	void WriteDoorMeshIndexBuffers(BufferView<int32_t> outAlphaTestedIndices);
}

#endif
