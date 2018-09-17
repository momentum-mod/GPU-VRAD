#include "raytrace.h"
#include "math.h"

CRayTracingEnvironment RTEnv;

void CRayTracingEnvironment::AddTriangle(int32_t id, const Vector & v1, const Vector & v2, const Vector & v3, const Vector & color)
{
	AddTriangle(id, v1, v2, v3, color, 0, 0);
}

void CRayTracingEnvironment::AddTriangle(int32_t id, const Vector & v1, const Vector & v2, const Vector & v3, const Vector & color, uint16_t flags, int32_t materialIndex)
{
	CacheOptimizedTriangle tmptri;
	tmptri.m_Data.m_GeometryData.m_nTriangleID = id;
	tmptri.Vertex(0) = v1;
	tmptri.Vertex(1) = v2;
	tmptri.Vertex(2) = v3;
	tmptri.m_Data.m_GeometryData.m_nFlags = flags;
	OptimizedTriangleList.push_back(tmptri);
	if (!(Flags & RTE_FLAGS_DONT_STORE_TRIANGLE_COLORS))
		TriangleColors.push_back(color);
	if (!(Flags & RTE_FLAGS_DONT_STORE_TRIANGLE_MATERIALS))
		TriangleMaterials.push_back(materialIndex);
}

static Vector GetEdgeEquation(const Vector &p1, const Vector &p2, int c1, int c2, Vector InsidePoint)
{
	float nx = p1[c2] - p2[c2];
	float ny = p2[c1] - p1[c1];
	float d = -(nx*p1[c1] + ny*p1[c2]);
	// 	assert(fabs(nx*p1[c1]+ny*p1[c2]+d)<0.01);
	// 	assert(fabs(nx*p2[c1]+ny*p2[c2]+d)<0.01);

	// use the convention that negative is "outside"
	float trial_dist = InsidePoint[c1] * nx + InsidePoint[c2] * ny + d;
	if (trial_dist<0)
	{
		nx = -nx;
		ny = -ny;
		d = -d;
		trial_dist = -trial_dist;
	}
	nx /= trial_dist;										// scale so that it will be =1.0 at the oppositve vertex
	ny /= trial_dist;
	d /= trial_dist;

	return Vector(nx, ny, d);
}

void CacheOptimizedTriangle::ChangeIntoIntersectionFormat(void)
{
	// lose the vertices and use edge equations instead

	// grab the whole original triangle to we don't overwrite it
	TriGeometryData_t srcTri = m_Data.m_GeometryData;

	m_Data.m_IntersectData.m_nFlags = srcTri.m_nFlags;
	m_Data.m_IntersectData.m_nTriangleID = srcTri.m_nTriangleID;

	Vector p1 = srcTri.Vertex(0);
	Vector p2 = srcTri.Vertex(1);
	Vector p3 = srcTri.Vertex(2);

	Vector e1 = p2 - p1;
	Vector e2 = p3 - p1;

	Vector N = e1.Cross(e2);
	N.NormalizeInPlace();
	// now, determine which axis to drop
	int drop_axis = 0;
	for (int c = 1; c<3; c++)
		if (fabs(N[c]) > fabs(N[drop_axis]))
			drop_axis = c;

	m_Data.m_IntersectData.m_flD = N.Dot(p1);
	m_Data.m_IntersectData.m_flNx = N.x;
	m_Data.m_IntersectData.m_flNy = N.y;
	m_Data.m_IntersectData.m_flNz = N.z;

	// decide which axes to keep
	int nCoordSelect0 = (drop_axis + 1) % 3;
	int nCoordSelect1 = (drop_axis + 2) % 3;

	m_Data.m_IntersectData.m_nCoordSelect0 = nCoordSelect0;
	m_Data.m_IntersectData.m_nCoordSelect1 = nCoordSelect1;


	Vector edge1 = GetEdgeEquation(p1, p2, nCoordSelect0, nCoordSelect1, p3);
	m_Data.m_IntersectData.m_ProjectedEdgeEquations[0] = edge1.x;
	m_Data.m_IntersectData.m_ProjectedEdgeEquations[1] = edge1.y;
	m_Data.m_IntersectData.m_ProjectedEdgeEquations[2] = edge1.z;

	Vector edge2 = GetEdgeEquation(p2, p3, nCoordSelect0, nCoordSelect1, p1);
	m_Data.m_IntersectData.m_ProjectedEdgeEquations[3] = edge2.x;
	m_Data.m_IntersectData.m_ProjectedEdgeEquations[4] = edge2.y;
	m_Data.m_IntersectData.m_ProjectedEdgeEquations[5] = edge2.z;
}

int CacheOptimizedTriangle::ClassifyAgainstAxisSplit(int split_plane, float split_value)
{
	// classify a triangle against an axis-aligned plane
	float minc = Vertex(0)[split_plane];
	float maxc = minc;
	for (int v = 1; v<3; v++)
	{
		minc = fmin(minc, Vertex(v)[split_plane]);
		maxc = fmax(maxc, Vertex(v)[split_plane]);
	}

	if (minc >= split_value)
		return PLANECHECK_POSITIVE;
	if (maxc <= split_value)
		return PLANECHECK_NEGATIVE;
	if (minc == maxc)
		return PLANECHECK_POSITIVE;
	return PLANECHECK_STRADDLING;
}
