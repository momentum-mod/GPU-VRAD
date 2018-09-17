#include "trace.h"
#include <algorithm>

CTrace::CTrace(const char * filename) : CBSP(filename)
{

}

void CTrace::AddBrushesForRayTrace()
{
	CTimer timer("AddBrushesForRayTrace");

	if (!m_iNumModels)
	{
		pp::Warning("No models found!");
		return;
	}

	VMatrix identity;
	identity.Identity();

	std::vector<int>brush_list;
	GetBrushesRecursively(m_dModels[0].headnode, brush_list);

	for (int i = 0; i < brush_list.size() && i < MAX_MAP_BRUSHES; i++)
	{
		dbrush_t *brush = &m_dBrushes[brush_list[i]];
		AddBrushToRaytraceEnvironment(brush, identity);
	}

	for (int i = 0; i < m_dModels[0].numfaces; i++)
	{
		int ndxFace = m_dModels[0].firstface + i;

		if (ndxFace < 0 || ndxFace > MAX_MAP_FACES)
		{
			pp::Error("Invalid face index %i", ndxFace);
			return;
		}

		dface_t *face = &m_pFaces[ndxFace];
		
		int texinfo_index = face->texinfo;

		if (texinfo_index < 0 || texinfo_index > MAX_MAP_TEXINFO)
		{
			pp::Error("Invalid texinfo index %i", texinfo_index);
			return;
		}

		texinfo_t *tx = &m_dTexInfos[face->texinfo];
		if (!(tx->flags & SURF_SKY))
			continue;

		Vector points[MAX_POINTS_ON_WINDING];

		for (int j = 0; j < face->numedges; j++)
		{
			int surf_edge_index = face->firstedge + j;
			if (j >= MAX_POINTS_ON_WINDING || j < 0)
			{
				pp::Error("MAX_POINTS_ON_WINDING reached!");
				return;
			}

			if (surf_edge_index > MAX_MAP_SURFEDGES || surf_edge_index < 0)
			{
				pp::Error("Invalid surf edge index %i", surf_edge_index);
				return;
			}

			int surf_edge = m_dSurfEdges[surf_edge_index];

			unsigned short v;

			if (surf_edge < 0)
				v = m_dEdges[-surf_edge].v[1];
			else
				v = m_dEdges[surf_edge].v[0];

			if (v > MAX_MAP_VERTS || v < 0)
			{
				pp::Error("Invalid vertex index %i", v);
				return;
			}

			dvertex_t *dv = &m_dVertexes[v];
			points[j] = dv->point;
		}

		for (int j = 2; j < face->numedges; j++)
		{
			Vector fullCoverage;
			fullCoverage.x = 1.0f;
			RTEnv.AddTriangle(TRACE_ID_SKY, points[0], points[j - 1], points[j], fullCoverage);
		}
	}
}

void CTrace::GetBrushesRecursively(int node, std::vector<int>& list)
{
	if (node < 0)
	{
		int leaf_index = -1 - node;

		if (leaf_index < 0 || leaf_index > MAX_MAP_LEAFS)
		{
			pp::Error("Invalid leaf index %i", leaf_index);
			return;
		}

		for (int i = 0; i < m_dLeafs[leaf_index].numleafbrushes; i++)
		{
			int leaf_brush_index = m_dLeafs[leaf_index].firstleafbrush + i;

			if (leaf_brush_index < 0 || leaf_brush_index > MAX_MAP_LEAFBRUSHES)
			{
				pp::Error("Invalid leaf brush index index %i", leaf_index);
				return;
			}

			int brush_index = m_dLeafBrushes[leaf_brush_index];

			if (brush_index < 0 || brush_index > MAX_MAP_BRUSHES)
			{
				pp::Error("Invalid brush index index %i", brush_index);
				return;
			}
			
			if (std::find(list.begin(), list.end(), brush_index) == list.end())
			{
				list.push_back(brush_index);
			}
		}
	}
	else
	{
		// recurse
		dnode_t *pnode = m_dNodes + node;

		GetBrushesRecursively(pnode->children[0], list);
		GetBrushesRecursively(pnode->children[1], list);
	}
}

void CTrace::AddBrushToRaytraceEnvironment(dbrush_t * brush, VMatrix & xform)
{
	if (!(brush->contents & MASK_OPAQUE))
		return;

	Vector v0, v1, v2;
	
	for (int i = 0; i < brush->numsides; i++)
	{
		int brush_side_index = brush->firstside + i;

		if (brush_side_index < 0 || brush_side_index > MAX_MAP_BRUSHSIDES)
		{
			pp::Error("Invalid brush side index index %i", brush_side_index);
			return;
		}

		dbrushside_t *side = &m_dBrushSides[brush->firstside + i];

		int plane_index = side->planenum;

		if (plane_index < 0 || plane_index > MAX_MAP_PLANES)
		{
			pp::Error("Invalid plade index index %i", plane_index);
			return;
		}

		dplane_t *plane = &m_dPlanes[side->planenum];

		int texinfo_index = side->texinfo;

		if (texinfo_index < 0 || texinfo_index > MAX_MAP_TEXINFO)
		{
			pp::Error("Invalid texinfo index index %i", texinfo_index);
			return;
		}

		texinfo_t *tx = &m_dTexInfos[side->texinfo];

		winding_t *w = BaseWindingForPlane(plane->normal, plane->dist);

		if (tx->flags & SURF_SKY || side->dispinfo)
			continue;

		for (int j = 0; j<brush->numsides && w; j++)
		{
			if (i == j)
				continue;
			dbrushside_t *pOtherSide = &m_dBrushSides[brush->firstside + j];
			if (pOtherSide->bevel)
				continue;
			plane = &m_dPlanes[pOtherSide->planenum ^ 1];
			ChopWindingInPlace(&w, plane->normal, plane->dist, 0);
		}
		if (w)
		{
			for (int j = 2; j < w->numpoints; j++)
			{
				v0 = xform.VMul4x3(w->p[0]);
				v1 = xform.VMul4x3(w->p[j - 1]);
				v2 = xform.VMul4x3(w->p[j]);
				Vector fullCoverage;
				fullCoverage.x = 1.0f;
				RTEnv.AddTriangle(TRACE_ID_OPAQUE, v0, v1, v2, fullCoverage);
			}
			FreeWinding(w);
		}
	}
}
