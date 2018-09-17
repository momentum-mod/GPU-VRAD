#include "bsp.h"

CBSP::CBSP(const char * filename)
{
	pp::Info("CBSP::CBSP(const char * filename)");
	memset(&m_Lumps, 0, sizeof(m_Lumps));

	std::ifstream ifs(filename, std::ios::ate | std::ios::binary);

	if (!ifs)
	{
		pp::Error("Couldn't open bsp file %s.", filename);
	}

	std::streamsize size = ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	m_bspData = new uint8_t[size];
	m_bspHdr = (dheader_t*)m_bspData;

	if (ifs.read((char*)m_bspData, size))
	{
		pp::Success("Loaded %lli bytes from %s", size, filename);
	}

	m_szFilename = filename;
	ifs.close();
}

CBSP::~CBSP()
{
	pp::Info("CBSP::~CBSP()");
	delete[] m_bspData;
}

void CBSP::LoadBSP()
{
	pp::Info("LoadBSP()");
	m_iMapRevision = m_bspHdr->mapRevision;

	if (!ValidateHeader(m_bspHdr))
	{
		return;
	}

	pp::Success("Found valid BSP header - map revision %i", m_iMapRevision);

	CopyLump(LUMP_ENTITIES, m_vEntData); // 0
	m_iNumPlanes = CopyLump(LUMP_PLANES, m_dPlanes, sizeof(*m_dPlanes), MAX_MAP_PLANES); // 1
	m_iNumTexData = CopyLump(LUMP_TEXDATA, m_dTexData, sizeof(*m_dTexData), MAX_MAP_TEXDATA); // 1
	m_iNumVertexes = CopyLump(LUMP_VERTEXES, m_dVertexes, sizeof(*m_dVertexes), MAX_MAP_VERTS); // 3
	m_iVisDataSize = CopyLump(LUMP_VISIBILITY, m_dVisData, sizeof(*m_dVisData), MAX_MAP_VISIBILITY); // 4
	m_dVis = (dvis_t *)m_dVisData;
	m_iNumNodes = CopyLump(LUMP_NODES, m_dNodes, sizeof(*m_dNodes), MAX_MAP_NODES); // 5
	m_iNumTexInfos = CopyLump(LUMP_TEXINFO, m_dTexInfos, sizeof(*m_dTexInfos), MAX_MAP_TEXINFO); // 6
	m_iNumFaces = CopyLump(LUMP_FACES, m_dFaces, sizeof(*m_dFaces), MAX_MAP_FACES); // 7
	CopyLump(LUMP_LIGHTING, m_vLightDataLDR); // 8

	m_iNumLeafs = CopyLump(LUMP_LEAFS, m_dLeafs, sizeof(*m_dLeafs), MAX_MAP_LEAFS); // 10
	CopyLump(LUMP_FACEIDS, m_vFaceIds); // 11
	m_iNumEdges = CopyLump(LUMP_EDGES, m_dEdges, sizeof(*m_dEdges), MAX_MAP_EDGES); // 12
	m_iNumSurfEdges = CopyLump(LUMP_SURFEDGES, m_dSurfEdges, sizeof(*m_dSurfEdges), MAX_MAP_SURFEDGES); // 13
	m_iNumModels = CopyLump(LUMP_MODELS, m_dModels, sizeof(*m_dModels), MAX_MAP_MODELS); // 14
	m_iNumWorldLightsLDR = CopyLump(LUMP_WORLDLIGHTS, m_dWorldLightsLDR, sizeof(*m_dWorldLightsLDR), MAX_MAP_WORLDLIGHTS); // 15
	m_iNumLeafFaces = CopyLump(LUMP_LEAFFACES, m_dLeafFaces, sizeof(*m_dLeafFaces), MAX_MAP_LEAFFACES); // 16
	m_iNumLeafBrushes = CopyLump(LUMP_LEAFBRUSHES, m_dLeafBrushes, sizeof(*m_dLeafBrushes), MAX_MAP_LEAFBRUSHES); // 17
	m_iNumBrushes = CopyLump(LUMP_BRUSHES, m_dBrushes, sizeof(*m_dBrushes), MAX_MAP_BRUSHES); // 18
	m_iNumBrushSides = CopyLump(LUMP_BRUSHSIDES, m_dBrushSides, sizeof(*m_dBrushSides), MAX_MAP_BRUSHSIDES); // 19
	m_iNumAreas = CopyLump(LUMP_AREAS, m_dAreas, sizeof(*m_dAreas), MAX_MAP_AREAS); // 20
	m_iNumAreaPortals = CopyLump(LUMP_AREAPORTALS, m_dAreaPortals, sizeof(*m_dAreaPortals), MAX_MAP_AREAPORTALS); // 21

	CopyLump(LUMP_DISPINFO, m_vDispInfos); // 26

	m_iNumOrigFaces = CopyLump(LUMP_ORIGINALFACES, m_dOrigFaces, sizeof(*m_dOrigFaces), MAX_MAP_FACES); // 27

	CopyLump(LUMP_PHYSDISP, m_vPhysDisp); // 28
	CopyLump(LUMP_PHYSCOLLIDE, m_vPhysCollide); // 29
	m_iNumVertNormals = CopyLump(LUMP_VERTNORMALS, m_dVertNormals, sizeof(*m_dVertNormals), MAX_MAP_VERTNORMALS); // 30
	m_iNumVertNormalIndices = CopyLump(LUMP_VERTNORMALINDICES, m_dVertNormalIndices, sizeof(*m_dVertNormalIndices), MAX_MAP_VERTNORMALS); // 31

	CopyLump(LUMP_DISP_VERTS, m_vDispVerts); // 33
	CopyLump(LUMP_DISP_LIGHTMAP_SAMPLE_POSITIONS, m_vNumDispLightmapSamplePositions); // 34

	m_iNumLeafWaterData = CopyLump(LUMP_LEAFWATERDATA, m_dLeafWaterData, sizeof(*m_dLeafWaterData), MAX_MAP_LEAFWATERDATA); // 36
	m_iNumPrimitives = CopyLump(LUMP_PRIMITIVES, m_dPrimitives, sizeof(*m_dPrimitives), MAX_MAP_PRIMITIVES); // 37
	m_iNumPrimverts = CopyLump(LUMP_PRIMVERTS, m_dPrimverts, sizeof(*m_dPrimverts), MAX_MAP_PRIMVERTS); // 38
	m_iNumPrimindices = CopyLump(LUMP_PRIMINDICES, m_dPrimindices, sizeof(*m_dPrimindices), MAX_MAP_PRIMINDICES); // 39
	// LUMP_PAKFILE TODO
	m_iNumClipPortalVerts = CopyLump(LUMP_CLIPPORTALVERTS, m_dClipPortalVerts, sizeof(*m_dClipPortalVerts), MAX_MAP_PORTALVERTS); // 41
	m_iNumCubemapSamples = CopyLump(LUMP_CUBEMAPS, m_dCubemapSamples, sizeof(*m_dCubemapSamples), MAX_MAP_CUBEMAPSAMPLES); // 42
	CopyLump(LUMP_TEXDATA_STRING_DATA, m_vTexDataStringData); // 43
	CopyLump(LUMP_TEXDATA_STRING_TABLE, m_vTexDataStringTable); // 44
	m_iNumOverlayCount = CopyLump(LUMP_OVERLAYS, m_dOverlays, sizeof(*m_dOverlays), MAX_MAP_OVERLAYS); // 45
	m_iNumLeafMinDistToWater = CopyLump(LUMP_LEAFMINDISTTOWATER, m_dLeafMinDistToWater, sizeof(*m_dLeafMinDistToWater), MAX_MAP_LEAFS); // 46

	CopyLump(LUMP_FACE_MACRO_TEXTURE_INFO, m_vFaceMacroTextureInfos); // 47
	CopyLump(LUMP_DISP_TRIS, m_vDispTris); // 48

	m_iNumWaterOverlayCount = CopyLump(LUMP_WATEROVERLAYS, m_dWaterOverlays, sizeof(*m_dWaterOverlays), MAX_MAP_WATEROVERLAYS); // 50
	CopyLump(LUMP_LEAF_AMBIENT_INDEX_HDR, m_vLeafAmbientIndexHDR); // 51
	CopyLump(LUMP_LEAF_AMBIENT_INDEX, m_vLeafAmbientIndexLDR); // 52
	CopyLump(LUMP_LIGHTING_HDR, m_vLightDataHDR); // 53
	m_iNumWorldLightsHDR = CopyLump(LUMP_WORLDLIGHTS_HDR, m_dWorldLightsHDR, sizeof(*m_dWorldLightsHDR), MAX_MAP_WORLDLIGHTS); // 39
	CopyLump(LUMP_LEAF_AMBIENT_LIGHTING_HDR, m_vLeafAmbientLightingHDR); // 55
	CopyLump(LUMP_LEAF_AMBIENT_LIGHTING, m_vLeafAmbientLightingLDR); // 56

	m_iNumFacesHDR = CopyLump(LUMP_FACES_HDR, m_dFacesHDR, sizeof(*m_dFacesHDR), MAX_MAP_FACES); // 58
	CopyLump(LUMP_MAP_FLAGS, &m_fLevelFlags, sizeof(uint32_t), 1);
	m_iNumOverlayFades = CopyLump(LUMP_OVERLAY_FADES, m_dOverlayFades, sizeof(*m_dOverlayFades), MAX_MAP_OVERLAYS); // 60

	// Gather unparsed lumps
	for (int i = 0; i < HEADER_LUMPS; i++)
	{
		if (!m_Lumps.bLumpParsed[i] && m_bspHdr->lumps[i].filelen)
		{
			m_Lumps.size[i] = CopyVariableLump<uint8_t>(i, &m_Lumps.pLumps[i]);
			pp::Warning("Unsaved lump index %i (%i bytes)", i, m_Lumps.size[i]);
		}
	}
}

bool CBSP::ValidateHeader(dheader_t* hdr)
{
	if (hdr->ident != IDBSPHEADER)
	{
		pp::Error("%s is not a IBSP file", m_szFilename);
		return false;
	}
	if (hdr->version < MINBSPVERSION || hdr->version > BSPVERSION)
	{
		pp::Error("%s is version %i, not %i", m_szFilename, hdr->version, BSPVERSION);
		return false;
	}
	return true;
}

int CBSP::CopyLump(int lump, void * out, size_t element_size, size_t max_count, int force_version)
{
	if (lump >= MAX_LUMPS || lump < 0)
	{
		pp::Error("Lump index out of bounds");
		return 0;
	}

	m_Lumps.bLumpParsed[lump] = true;

	if (!HasLump(lump))
	{
		memset(out, 0, max_count * element_size);
		pp::Warning("[%s] Missing lump index %i", g_szLumpNames[lump], lump);
		return 0;
	}

	size_t length = m_bspHdr->lumps[lump].filelen;
	size_t ofs = m_bspHdr->lumps[lump].fileofs;
	size_t count = length / element_size;
	
	if (max_count < count)
	{
		pp::Error("[%s] Lump index %i is using %i/%i elements", g_szLumpNames[lump], lump, count, max_count);
		return 0;
	}
	if (length % element_size)
	{
		pp::Error("[%s] Lump index %i has incorrect file length", g_szLumpNames[lump], lump);
		return 0;
	}
	if (force_version >= 0 && force_version != m_bspHdr->lumps[lump].version)
	{
		pp::Error("[%s] Old version for lump %i", g_szLumpNames[lump], lump);
		return 0;
	}

	pp::Success("[%s] Loaded lump index %i (Size: %i, File offset: %i, Items: %i, Item size: %i)", g_szLumpNames[lump], lump, length, ofs, count, element_size);

	memcpy(out, (byte*)m_bspData + ofs, length);

	return count;
}

bool CBSP::HasLump(int lump)
{
	return m_bspHdr->lumps[lump].filelen > 0;
}

int CBSP::LumpVersion(int lump)
{
	return m_bspHdr->lumps[lump].version;
}

void CBSP::ParseEntities()
{
	m_iNumEntities = 0;
	ParseFromMemory(m_vEntData.data(), m_vEntData.size());

	while (ParseEntity())
	{
	}

	pp::Success("Found %i entities", m_iNumEntities);
}

qboolean CBSP::ParseEntity()
{
	epair_t		*e;
	entity_t	*mapent;

	if (!GetToken(true))
		return false;

	if (_stricmp(token, "{"))
		pp::Error("ParseEntity: { not found");

	if (m_iNumEntities > MAX_MAP_ENTITIES)
		pp::Error("num_entities > MAX_MAP_ENTITIES");

	mapent = &m_Entities[m_iNumEntities];
	m_iNumEntities++;

	do
	{
		if (!GetToken(true))
			pp::Error("ParseEntity: EOF without closing brace");

		if (!_stricmp(token, "}"))
			break;

		e = ParseEpair();
		e->next = mapent->epairs;
		mapent->epairs = e;
	} while (1);
	
	return true;
}


epair_t * CBSP::ParseEpair()
{
	epair_t	*e;

	e = (epair_t*)malloc(sizeof(epair_t));
	memset(e, 0, sizeof(epair_t));

	if (strlen(token) >= MAX_KEY - 1)
		pp::Error("ParseEpar: token too long");
	e->key = _strdup(token);

	GetToken(false);
	if (strlen(token) >= MAX_VALUE - 1)
		pp::Error("ParseEpar: token too long");
	e->value = _strdup(token);

	// strip trailing spaces
	StripTrailing(e->key);
	StripTrailing(e->value);

	return e;
}

void CBSP::StripTrailing(char *e)
{
	char	*s;

	s = e + strlen(e) - 1;
	while (s >= e && *s <= 32)
	{
		*s = 0;
		s--;
	}
}