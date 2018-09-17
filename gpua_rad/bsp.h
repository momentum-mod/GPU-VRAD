#ifndef _GPUA_BSP_H_
#define _GPUA_BSP_H_

#include <string.h>
#include <stdint.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "script.h"

#include "prettyprint.h"

#include "vector.h"
#include "bspfile.h"

#define	MAX_TEXLIGHTS			128
#define MAX_TEXLIGHT_NAME		256

class CBSP : public CScript
{
public:
	CBSP(const char* filename);
	~CBSP();
	void LoadBSP();
	bool ValidateHeader(dheader_t* hdr);
	int CopyLump(int lump, void* out, size_t element_size, size_t max_count, int force_version = -1);

	template<class T>
	int CopyLump(int lump, std::vector<T> &vec, int force_version = -1)
	{
		size_t count = m_bspHdr->lumps[lump].filelen / sizeof(T);
		vec.resize(count);
		return CopyLump(lump, vec.data(), sizeof(T), count, force_version);
	}
	
	template<class T>
	int CopyVariableLump(int lump, void** dest, int force_version = -1)
	{
		size_t count = m_bspHdr->lumps[lump].filelen / sizeof(T);
		*dest = malloc(count * sizeof(T));

		return CopyLump(lump, *dest, sizeof(T), count, force_version);
	}

	void ParseEntities();
	qboolean ParseEntity();
	epair_t* ParseEpair();
	void StripTrailing(char *e);

	bool HasLump(int lump);
	int LumpVersion(int lump);
	// Filename
	const char* m_szFilename;

	// Bsp stuff
	lumps_t m_Lumps;
	uint8_t* m_bspData;
	dheader_t* m_bspHdr;
	int m_iMapRevision;
	uint32_t m_fLevelFlags;

	/// Lumps
	// lump 0
	std::vector<char> m_vEntData;

	// lump 1
	int m_iNumPlanes;
	dplane_t m_dPlanes[MAX_MAP_PLANES];

	// lump 2
	int m_iNumTexData;
	dtexdata_t m_dTexData[MAX_MAP_TEXDATA];

	// lump 3
	int m_iNumVertexes;
	dvertex_t m_dVertexes[MAX_MAP_VERTS];

	// lump 4
	int m_iVisDataSize;
	uint8_t m_dVisData[MAX_MAP_VISIBILITY];
	dvis_t *m_dVis;

	// lump 5
	int m_iNumNodes;
	dnode_t m_dNodes[MAX_MAP_NODES];

	// lump 6
	int m_iNumTexInfos;
	texinfo_t m_dTexInfos[MAX_MAP_TEXINFO];

	// lump 7
	int m_iNumFaces;
	dface_t m_dFaces[MAX_MAP_FACES];

	// lump 8
	std::vector<uint8_t> m_vLightDataLDR;

	// lump 10
	int m_iNumLeafs;
	dleaf_t m_dLeafs[MAX_MAP_LEAFS];

	// lump 11
	std::vector<dfaceid_t> m_vFaceIds;

	// lump 12
	int m_iNumEdges;
	dedge_t m_dEdges[MAX_MAP_EDGES];

	// lump 13
	int m_iNumSurfEdges;
	int m_dSurfEdges[MAX_MAP_SURFEDGES];

	// lump 14
	int m_iNumModels;
	dmodel_t m_dModels[MAX_MAP_MODELS];

	// lump 15
	int m_iNumWorldLightsLDR;
	dworldlight_t m_dWorldLightsLDR[MAX_MAP_WORLDLIGHTS];

	// lump 16
	int m_iNumLeafFaces;
	unsigned short m_dLeafFaces[MAX_MAP_LEAFFACES];

	// lump 17
	int m_iNumLeafBrushes;
	unsigned short m_dLeafBrushes[MAX_MAP_LEAFBRUSHES];

	// lump 18
	int m_iNumBrushes;
	dbrush_t m_dBrushes[MAX_MAP_BRUSHES];

	// lump 19
	int m_iNumBrushSides;
	dbrushside_t m_dBrushSides[MAX_MAP_BRUSHSIDES];

	// lump 20
	int m_iNumAreas;
	darea_t m_dAreas[MAX_MAP_AREAS];

	// lump 21
	int m_iNumAreaPortals;
	dareaportal_t m_dAreaPortals[MAX_MAP_AREAPORTALS];

	// lump 26
	std::vector<ddispinfo_t> m_vDispInfos;

	// lump 27
	int m_iNumOrigFaces;
	dface_t m_dOrigFaces[MAX_MAP_FACES];

	// lump 28
	std::vector<uint8_t> m_vPhysDisp;

	// lump 29
	std::vector<uint8_t> m_vPhysCollide;

	// lump 30
	int m_iNumVertNormals;
	Vector m_dVertNormals[MAX_MAP_VERTNORMALS];

	// lump 31
	int m_iNumVertNormalIndices; // dfaces reference these. These index g_vertnormals.
	unsigned short m_dVertNormalIndices[MAX_MAP_VERTNORMALS];

	// lump 33
	std::vector<CDispVert> m_vDispVerts;

	// lump 34
	std::vector<unsigned char> m_vNumDispLightmapSamplePositions;

	// lump 36
	int m_iNumLeafWaterData;
	dleafwaterdata_t m_dLeafWaterData[MAX_MAP_LEAFWATERDATA];

	// lump 37
	int m_iNumPrimitives;
	dprimitive_t m_dPrimitives[MAX_MAP_PRIMITIVES];

	// lump 38
	int m_iNumPrimverts;
	dprimvert_t m_dPrimverts[MAX_MAP_PRIMVERTS];

	// lump 39
	int m_iNumPrimindices;
	unsigned short m_dPrimindices[MAX_MAP_PRIMINDICES];

	// lump 41
	int m_iNumClipPortalVerts;
	Vector m_dClipPortalVerts[MAX_MAP_PORTALVERTS];

	// lump 42
	int m_iNumCubemapSamples = 0;
	dcubemapsample_t m_dCubemapSamples[MAX_MAP_CUBEMAPSAMPLES];

	// lump 43
	std::vector<char> m_vTexDataStringData;

	// lump 44
	std::vector<int> m_vTexDataStringTable;

	// lump 45
	int m_iNumOverlayCount;
	doverlay_t m_dOverlays[MAX_MAP_OVERLAYS];

	// lump 46
	int m_iNumLeafMinDistToWater;
	unsigned short m_dLeafMinDistToWater[MAX_MAP_LEAFS];

	// lump 47
	std::vector<CDispTri> m_vFaceMacroTextureInfos;

	// lump 48
	std::vector<CDispTri> m_vDispTris;

	// lump 50
	int m_iNumWaterOverlayCount;
	dwateroverlay_t m_dWaterOverlays[MAX_MAP_WATEROVERLAYS];

	// lump 51
	std::vector<dleafambientindex_t> m_vLeafAmbientIndexHDR;

	// lump 52
	std::vector<dleafambientindex_t> m_vLeafAmbientIndexLDR;

	// lump 53
	std::vector<uint8_t> m_vLightDataHDR;

	// lump 54
	int m_iNumWorldLightsHDR;
	dworldlight_t m_dWorldLightsHDR[MAX_MAP_WORLDLIGHTS];

	// lump 55
	std::vector<dleafambientlighting_t> m_vLeafAmbientLightingHDR;

	// lump 56
	std::vector<dleafambientlighting_t> m_vLeafAmbientLightingLDR;

	// lump 58
	int m_iNumFacesHDR;
	dface_t m_dFacesHDR[MAX_MAP_FACES];

	// lump 60
	int m_iNumOverlayFades;
	doverlayfade_t		m_dOverlayFades[MAX_MAP_OVERLAYS];

	int m_iNumEntities;
	entity_t m_Entities[MAX_MAP_ENTITIES];
};

#endif // _GPUA_BSP_H_