#ifndef _GPUARAD_RAYTRACE_H_
#define _GPUARAD_RAYTRACE_H_

#define RTE_FLAGS_FAST_TREE_GENERATION 1
#define RTE_FLAGS_DONT_STORE_TRIANGLE_COLORS 2				// saves memory if not needed
#define RTE_FLAGS_DONT_STORE_TRIANGLE_MATERIALS 4

#define PLANECHECK_POSITIVE 1
#define PLANECHECK_NEGATIVE -1
#define PLANECHECK_STRADDLING 0

#define KDNODE_STATE_XSPLIT 0								// this node is an x split
#define KDNODE_STATE_YSPLIT 1								// this node is a ysplit
#define KDNODE_STATE_ZSPLIT 2								// this node is a zsplit
#define KDNODE_STATE_LEAF 3									// this node is a leaf

#include <stdint.h>

#include <vector>

#include "vector.h"

struct TriIntersectData_t
{
	// this structure is 16longs=64 bytes for cache line packing.
	float m_flNx, m_flNy, m_flNz;							// plane equation
	float m_flD;

	int32_t m_nTriangleID;									// id of the triangle.

	float m_ProjectedEdgeEquations[6];						// A,B,C for each edge equation.  a
															// point is inside the triangle if
															// a*c1+b*c2+c is negative for all 3
															// edges.

	uint8_t m_nCoordSelect0, m_nCoordSelect1;				// the triangle is projected onto a 2d
															// plane for edge testing. These are
															// the indices (0..2) of the
															// coordinates preserved in the
															// projection

	uint8_t m_nFlags;										// triangle flags
	uint8_t m_unused0;										// no longer used
};


struct TriGeometryData_t
{
	int32_t m_nTriangleID;									// id of the triangle.

	float m_VertexCoordData[9];								// can't use a vector in a union

	uint8_t m_nFlags;										// triangle flags
	signed char m_nTmpData0;								// used by kd-tree builder
	signed char m_nTmpData1;								// used by kd-tree builder


															// accessors to get around union annoyance
	FORCEINLINE Vector &Vertex(int idx)
	{
		return *(reinterpret_cast<Vector *> (m_VertexCoordData + 3 * idx));
	}

};

struct CacheOptimizedTriangle
{

	union
	{
		TriIntersectData_t m_IntersectData;
		TriGeometryData_t m_GeometryData;
	} m_Data;

	// accessors to get around union annoyance
	FORCEINLINE Vector &Vertex(int idx)
	{
		return *(reinterpret_cast<Vector *> (m_Data.m_GeometryData.m_VertexCoordData + 3 * idx));
	}

	FORCEINLINE const Vector &Vertex(int idx) const
	{
		return *(reinterpret_cast<const Vector *> (m_Data.m_GeometryData.m_VertexCoordData + 3 * idx));
	}

	void ChangeIntoIntersectionFormat(void);				// change information storage format for
															// computing intersections.

	int ClassifyAgainstAxisSplit(int split_plane, float split_value); // PLANECHECK_xxx below

};

class CRayTracingEnvironment
{
public:
	uint32_t Flags;											// RTE_F
	std::vector<CacheOptimizedTriangle> OptimizedTriangleList; //< the packed trianglesLAGS_xxx above
	std::vector<Vector> TriangleColors;						//< color of tries
	std::vector<int32_t> TriangleMaterials;					//< material index of tries


	CRayTracingEnvironment()
	{
		Flags = 0;
	}

	// call AddTriangle to set up the world
	void AddTriangle(int32_t id, const Vector &v1, const Vector &v2, const Vector &v3,
		const Vector &color);

	// Adds a triangle with flags & material
	void AddTriangle(int32_t id, const Vector &v1, const Vector &v2, const Vector &v3,
		const Vector &color, uint16_t flags, int32_t materialIndex);

};

extern CRayTracingEnvironment RTEnv;

#endif // _GPUARAD_RAYTRACE_H_