#ifndef _GPUA_BSPFILE_H_
#define _GPUA_BSPFILE_H_

#include "datamap.h"

//#pragma pack(1)

#define IDBSPHEADER	(('P'<<24)+('S'<<16)+('B'<<8)+'V')

#define MINBSPVERSION 19
#define BSPVERSION 20

// key / value pair sizes
#define	MAX_KEY		32
#define	MAX_VALUE	1024

// level feature flags
#define LVLFLAGS_BAKED_STATIC_PROP_LIGHTING_NONHDR 0x00000001	// was processed by vrad with -staticproplighting, no hdr data
#define LVLFLAGS_BAKED_STATIC_PROP_LIGHTING_HDR    0x00000002   // was processed by vrad with -staticproplighting, in hdr

// upper design bounds
#define MIN_MAP_DISP_POWER		2	// Minimum and maximum power a displacement can be.
#define MAX_MAP_DISP_POWER		4	

// Max # of neighboring displacement touching a displacement's corner.
#define MAX_DISP_CORNER_NEIGHBORS	4

#define NUM_DISP_POWER_VERTS(power)	( ((1 << (power)) + 1) * ((1 << (power)) + 1) )
#define NUM_DISP_POWER_TRIS(power)	( (1 << (power)) * (1 << (power)) * 2 )

#define MAX_MAP_DISP_VERTS				( MAX_MAP_DISPINFO * ((1<<MAX_MAP_DISP_POWER)+1) * ((1<<MAX_MAP_DISP_POWER)+1))
#define MAX_MAP_DISP_TRIS				( (1 << MAX_MAP_DISP_POWER) * (1 << MAX_MAP_DISP_POWER) * 2 )
#define MAX_DISPVERTS					NUM_DISP_POWER_VERTS( MAX_MAP_DISP_POWER )
#define MAX_DISPTRIS					NUM_DISP_POWER_TRIS( MAX_MAP_DISP_POWER )

// Pad a number so it lies on an N byte boundary.
// So PAD_NUMBER(0,4) is 0 and PAD_NUMBER(1,4) is 4
#define PAD_NUMBER(number, boundary) \
	( ((number) + ((boundary)-1)) / (boundary) ) * (boundary)

// Maximum lump sizes
#define	MAX_MAP_PLANES					65536						// lump 1
#define MAX_MAP_TEXDATA					2048						// lump 2
#define	MAX_MAP_VERTS					65536						// lump 3
#define	MAX_MAP_VISIBILITY				0x1000000					// lump 4
#define	MAX_MAP_NODES					65536						// lump 5
#define	MAX_MAP_TEXINFO					12288						// lump 6
#define	MAX_MAP_FACES					65536						// lump 7
#define	MAX_MAP_LEAFS					65536						// lump 10
#define	MAX_MAP_EDGES					256000						// lump 12
#define	MAX_MAP_SURFEDGES				512000						// lump 13
#define	MAX_MAP_MODELS					1024						// lump 14
#define MAX_MAP_WORLDLIGHTS				8192						// lump 15
#define	MAX_MAP_LEAFFACES				65536						// lump 16
#define	MAX_MAP_LEAFBRUSHES 			65536						// lump 17
#define	MAX_MAP_BRUSHES					8192						// lump 18
#define	MAX_MAP_BRUSHSIDES				65536						// lump 19
#define	MAX_MAP_AREAS					256							// lump 20
#define	MAX_MAP_AREAPORTALS				1024						// lump 21
#define MAX_MAP_DISPINFO				2048						// lump 26
#define MAX_MAP_VERTNORMALS				256000						// lump 30
#define MAX_MAP_LEAFWATERDATA			32768						// lump 36
#define MAX_MAP_PRIMITIVES				32768						// lump 37
#define MAX_MAP_PRIMVERTS				65536						// lump 38
#define MAX_MAP_PRIMINDICES				65536						// lump 39
#define MAX_MAP_PORTALVERTS				128000						// lump 41
#define MAX_MAP_CUBEMAPSAMPLES			1024						// lump 42
#define MAX_MAP_OVERLAYS				512							// lump 45
#define MAX_MAP_WATEROVERLAYS			16384						// lump 50

#define	MAX_MAP_ENTITIES				8192
#define MAX_MAP_CLUSTERS				65536

static const char *g_szLumpNames[] = {
	"LUMP_ENTITIES",						// 0
	"LUMP_PLANES",							// 1
	"LUMP_TEXDATA",							// 2
	"LUMP_VERTEXES",						// 3
	"LUMP_VISIBILITY",						// 4
	"LUMP_NODES",							// 5
	"LUMP_TEXINFO",							// 6
	"LUMP_FACES",							// 7
	"LUMP_LIGHTING",						// 8
	"LUMP_OCCLUSION",						// 9
	"LUMP_LEAFS",							// 10
	"LUMP_FACEIDS",							// 11
	"LUMP_EDGES",							// 12
	"LUMP_SURFEDGES",						// 13
	"LUMP_MODELS",							// 14
	"LUMP_WORLDLIGHTS",						// 15
	"LUMP_LEAFFACES",						// 16
	"LUMP_LEAFBRUSHES",						// 17
	"LUMP_BRUSHES",							// 18
	"LUMP_BRUSHSIDES",						// 19
	"LUMP_AREAS",							// 20
	"LUMP_AREAPORTALS",						// 21
	"LUMP_UNUSED0",							// 22
	"LUMP_UNUSED1",							// 23
	"LUMP_UNUSED2",							// 24
	"LUMP_UNUSED3",							// 25
	"LUMP_DISPINFO",						// 26
	"LUMP_ORIGINALFACES",					// 27
	"LUMP_PHYSDISP",						// 28
	"LUMP_PHYSCOLLIDE",						// 29
	"LUMP_VERTNORMALS",						// 30
	"LUMP_VERTNORMALINDICES",				// 31
	"LUMP_DISP_LIGHTMAP_ALPHAS",			// 32
	"LUMP_DISP_VERTS",						// 33
	"LUMP_DISP_LIGHTMAP_SAMPLE_POSITIONS",	// 34
	"LUMP_GAME_LUMP",						// 35
	"LUMP_LEAFWATERDATA",					// 36
	"LUMP_PRIMITIVES",						// 37
	"LUMP_PRIMVERTS",						// 38
	"LUMP_PRIMINDICES",						// 39
	"LUMP_PAKFILE",							// 40
	"LUMP_CLIPPORTALVERTS",					// 41
	"LUMP_CUBEMAPS",						// 42
	"LUMP_TEXDATA_STRING_DATA",				// 43
	"LUMP_TEXDATA_STRING_TABLE",			// 44
	"LUMP_OVERLAYS",						// 45
	"LUMP_LEAFMINDISTTOWATER",				// 46
	"LUMP_FACE_MACRO_TEXTURE_INFO",			// 47
	"LUMP_DISP_TRIS",						// 48
	"LUMP_PHYSCOLLIDESURFACE",				// 49
	"LUMP_WATEROVERLAYS",					// 50
	"LUMP_LEAF_AMBIENT_INDEX_HDR",			// 51
	"LUMP_LEAF_AMBIENT_INDEX",				// 52
	"LUMP_LIGHTING_HDR",					// 53
	"LUMP_WORLDLIGHTS_HDR",					// 54
	"LUMP_LEAF_AMBIENT_LIGHTING_HDR",		// 55
	"LUMP_LEAF_AMBIENT_LIGHTING",			// 56
	"LUMP_XZIPPAKFILE",						// 57
	"LUMP_FACES_HDR",						// 58
	"LUMP_MAP_FLAGS",						// 59
	"LUMP_OVERLAY_FADES",					// 60
};

enum
{
	LUMP_ENTITIES = 0,	// *
	LUMP_PLANES = 1,	// *
	LUMP_TEXDATA = 2,	// *
	LUMP_VERTEXES = 3,	// *
	LUMP_VISIBILITY = 4,	// *
	LUMP_NODES = 5,	// *
	LUMP_TEXINFO = 6,	// *
	LUMP_FACES = 7,	// *
	LUMP_LIGHTING = 8,	// *
	LUMP_OCCLUSION = 9,
	LUMP_LEAFS = 10,	// *
	LUMP_FACEIDS = 11,
	LUMP_EDGES = 12,	// *
	LUMP_SURFEDGES = 13,	// *
	LUMP_MODELS = 14,	// *
	LUMP_WORLDLIGHTS = 15,	// 
	LUMP_LEAFFACES = 16,	// *
	LUMP_LEAFBRUSHES = 17,	// *
	LUMP_BRUSHES = 18,	// *
	LUMP_BRUSHSIDES = 19,	// *
	LUMP_AREAS = 20,	// *
	LUMP_AREAPORTALS = 21,	// *
	LUMP_UNUSED0 = 22,
	LUMP_UNUSED1 = 23,
	LUMP_UNUSED2 = 24,
	LUMP_UNUSED3 = 25,
	LUMP_DISPINFO = 26,
	LUMP_ORIGINALFACES = 27,
	LUMP_PHYSDISP = 28,
	LUMP_PHYSCOLLIDE = 29,
	LUMP_VERTNORMALS = 30,
	LUMP_VERTNORMALINDICES = 31,
	LUMP_DISP_LIGHTMAP_ALPHAS = 32,
	LUMP_DISP_VERTS = 33,		// CDispVerts
	LUMP_DISP_LIGHTMAP_SAMPLE_POSITIONS = 34,	// For each displacement
												//     For each lightmap sample
												//         byte for index
												//         if 255, then index = next byte + 255
												//         3 bytes for barycentric coordinates
	// The game lump is a method of adding game-specific lumps
	// FIXME: Eventually, all lumps could use the game lump system
	LUMP_GAME_LUMP = 35,
	LUMP_LEAFWATERDATA = 36,
	LUMP_PRIMITIVES = 37,
	LUMP_PRIMVERTS = 38,
	LUMP_PRIMINDICES = 39,
	// A pak file can be embedded in a .bsp now, and the file system will search the pak
	//  file first for any referenced names, before deferring to the game directory 
	//  file system/pak files and finally the base directory file system/pak files.
	LUMP_PAKFILE = 40,
	LUMP_CLIPPORTALVERTS = 41,
	// A map can have a number of cubemap entities in it which cause cubemap renders
	// to be taken after running vrad.
	LUMP_CUBEMAPS = 42,
	LUMP_TEXDATA_STRING_DATA = 43,
	LUMP_TEXDATA_STRING_TABLE = 44,
	LUMP_OVERLAYS = 45,
	LUMP_LEAFMINDISTTOWATER = 46,
	LUMP_FACE_MACRO_TEXTURE_INFO = 47,
	LUMP_DISP_TRIS = 48,
	LUMP_PHYSCOLLIDESURFACE = 49,	// deprecated.  We no longer use win32-specific havok compression on terrain
	LUMP_WATEROVERLAYS = 50,
	LUMP_LEAF_AMBIENT_INDEX_HDR = 51,	// index of LUMP_LEAF_AMBIENT_LIGHTING_HDR
	LUMP_LEAF_AMBIENT_INDEX = 52,	// index of LUMP_LEAF_AMBIENT_LIGHTING

	// optional lumps for HDR
	LUMP_LIGHTING_HDR = 53,
	LUMP_WORLDLIGHTS_HDR = 54,
	LUMP_LEAF_AMBIENT_LIGHTING_HDR = 55,	// NOTE: this data overrides part of the data stored in LUMP_LEAFS.
	LUMP_LEAF_AMBIENT_LIGHTING = 56,	// NOTE: this data overrides part of the data stored in LUMP_LEAFS.

	LUMP_XZIPPAKFILE = 57,   // deprecated. xbox 1: xzip version of pak file
	LUMP_FACES_HDR = 58,	// HDR maps may have different face data.
	LUMP_MAP_FLAGS = 59,   // extended level-wide flags. not present in all levels
	LUMP_OVERLAY_FADES = 60,	// Fade distances for overlays
	MAX_LUMPS
};

#define	HEADER_LUMPS		64

struct lump_t
{
	int		fileofs, filelen;
	int		version;		// default to zero
							// this field was char fourCC[4] previously, but was unused, favoring the LUMP IDs above instead. It has been
							// repurposed for compression.  0 implies the lump is not compressed.
	int		uncompressedSize; // default to zero
};

struct dheader_t
{
	int			ident;
	int			version;
	lump_t		lumps[HEADER_LUMPS];
	int			mapRevision;				// the map's revision (iteration, version) number (added BSPVERSION 6)
};

struct lumps_t
{
	void	*pLumps[HEADER_LUMPS];
	int		size[HEADER_LUMPS];
	bool	bLumpParsed[HEADER_LUMPS];
};

struct dplane_t // lump 1
{
	Vector	normal;
	float	dist;
	int		type;		// PLANE_X - PLANE_ANYZ ?remove? trivial to regenerate
};

struct dtexdata_t // lump 2
{
	Vector		reflectivity;
	int			nameStringTableID;				// index into g_StringTable for the texture name
	int			width, height;					// source image
	int			view_width, view_height;		//
};

struct dvertex_t // lump 3
{
	Vector	point;
};

// the visibility lump consists of a header with a count, then
// byte offsets for the PVS and PHS of each cluster, then the raw
// compressed bit vectors
#define	DVIS_PVS	0
#define	DVIS_PAS	1
struct dvis_t // lump 4
{
	int			numclusters;
	int			bitofs[8][2];	// bitofs[numclusters][2]
};

struct dnode_t // lump 5
{
	int			planenum;
	int			children[2];	// negative numbers are -(leafs+1), not nodes
	short		mins[3];		// for frustom culling
	short		maxs[3];
	unsigned short	firstface;
	unsigned short	numfaces;	// counting both sides
	short			area;		// If all leaves below this node are in the same area, then
								// this is the area index. If not, this is -1.
};

struct texinfo_t // lump 6
{
	float		textureVecsTexelsPerWorldUnits[2][4];			// [s/t][xyz offset]
	float		lightmapVecsLuxelsPerWorldUnits[2][4];			// [s/t][xyz offset] - length is in units of texels/area
	int			flags;				// miptex flags + overrides
	int			texdata;			// Pointer to texture name, size, etc.
};

#define	MAXLIGHTMAPS	4

struct dface_t // lump 7
{
	unsigned short	planenum;
	byte		side;	// faces opposite to the node's plane direction
	byte		onNode; // 1 of on node, 0 if in leaf

	int			firstedge;		// we must support > 64k edges
	short		numedges;
	short		texinfo;
	// This is a union under the assumption that a fog volume boundary (ie. water surface)
	// isn't a displacement map.
	// FIXME: These should be made a union with a flags or type field for which one it is
	// if we can add more to this.
	//	union
	//	{
	short       dispinfo;
	// This is only for surfaces that are the boundaries of fog volumes
	// (ie. water surfaces)
	// All of the rest of the surfaces can look at their leaf to find out
	// what fog volume they are in.
	short		surfaceFogVolumeID;
	//	};

	// lighting info
	byte		styles[MAXLIGHTMAPS];
	int			lightofs;		// start of [numstyles*surfsize] samples
	float       area;

	// TODO: make these unsigned chars?
	int			m_LightmapTextureMinsInLuxels[2];
	int			m_LightmapTextureSizeInLuxels[2];

	int origFace;				// reference the original face this face was derived from


public:

	unsigned short GetNumPrims() const;
	void SetNumPrims(unsigned short nPrims);
	bool AreDynamicShadowsEnabled();
	void SetDynamicShadowsEnabled(bool bEnabled);

	// non-polygon primitives (strips and lists)
private:
	unsigned short m_NumPrims;	// Top bit, if set, disables shadows on this surface (this is why there are accessors).

public:
	unsigned short	firstPrimID;

	unsigned int	smoothingGroups;
};

inline unsigned short dface_t::GetNumPrims() const
{
	return m_NumPrims & 0x7FFF;
}

inline void dface_t::SetNumPrims(unsigned short nPrims)
{
	m_NumPrims &= ~0x7FFF;
	m_NumPrims |= (nPrims & 0x7FFF);
}

inline bool dface_t::AreDynamicShadowsEnabled()
{
	return (m_NumPrims & 0x8000) == 0;
}

inline void dface_t::SetDynamicShadowsEnabled(bool bEnabled)
{
	if (bEnabled)
		m_NumPrims &= ~0x8000;
	else
		m_NumPrims |= 0x8000;
}

struct dleaf_t // lump 10
{
	int				contents;			// OR of all brushes (not needed?)

	short			cluster;

	BEGIN_BITFIELD(bf);
	short			area : 9;
	short			flags : 7;			// Per leaf flags.
	END_BITFIELD();

	short			mins[3];			// for frustum culling
	short			maxs[3];

	unsigned short	firstleafface;
	unsigned short	numleaffaces;

	unsigned short	firstleafbrush;
	unsigned short	numleafbrushes;
	short			leafWaterDataID; // -1 for not in water

									 // NOTE: removed this for version 1 and moved into separate lump "LUMP_LEAF_AMBIENT_LIGHTING" or "LUMP_LEAF_AMBIENT_LIGHTING_HDR"
									 // Precaculated light info for entities.
									 //	CompressedLightCube m_AmbientLighting;
};

struct dfaceid_t // lump 11
{
	unsigned short	hammerfaceid;
};

struct dedge_t // lump 12
{
	unsigned short	v[2];		// vertex numbers
};

struct dmodel_t // lump 14
{
	Vector		mins, maxs;
	Vector		origin;					// for sounds or lights
	int			headnode;
	int			firstface, numfaces;	// submodels just draw faces without walking the bsp tree
};

// lights that were used to illuminate the world
enum emittype_t
{
	emit_surface,		// 90 degree spotlight
	emit_point,			// simple point light source
	emit_spotlight,		// spotlight with penumbra
	emit_skylight,		// directional light with no falloff (surface must trace to SKY texture)
	emit_quakelight,	// linear falloff, non-lambertian
	emit_skyambient,	// spherical light source with no falloff (surface must trace to SKY texture)
};

// Flags for dworldlight_t::flags
#define DWL_FLAGS_INAMBIENTCUBE		0x0001	// This says that the light was put into the per-leaf ambient cubes.

struct dworldlight_t // lump 15
{
	Vector		origin;
	Vector		intensity;
	Vector		normal;			// for surfaces and spotlights
	int			cluster;
	emittype_t	type;
	int			style;
	float		stopdot;		// start of penumbra for emit_spotlight
	float		stopdot2;		// end of penumbra for emit_spotlight
	float		exponent;		// 
	float		radius;			// cutoff distance
								// falloff for emit_spotlight + emit_point: 
								// 1 / (constant_attn + linear_attn * dist + quadratic_attn * dist^2)
	float		constant_attn;
	float		linear_attn;
	float		quadratic_attn;
	int			flags;			// Uses a combination of the DWL_FLAGS_ defines.
	int			texinfo;		// 
	int			owner;			// entity that this light it relative to
};
struct dbrush_t // lump 18
{
	int			firstside;
	int			numsides;
	int			contents;
};

struct dbrushside_t // lump 19
{
	unsigned short	planenum;		// facing out of the leaf
	short			texinfo;
	short			dispinfo;		// displacement info (BSPVERSION 7)
	short			bevel;			// is the side a bevel plane? (BSPVERSION 7)
};

struct darea_t // lump 20
{
	int		numareaportals;
	int		firstareaportal;
};

// each area has a list of portals that lead into other areas
// when portals are closed, other areas may not be visible or
// hearable even if the vis info says that it should be
struct dareaportal_t // lump 21
{
	unsigned short	m_PortalKey;		// Entities have a key called portalnumber (and in vbsp a variable
										// called areaportalnum) which is used
										// to bind them to the area portals by comparing with this value.

	unsigned short	otherarea;		// The area this portal looks into.

	unsigned short	m_FirstClipPortalVert;	// Portal geometry.
	unsigned short	m_nClipPortalVerts;

	int				planenum;
};

// These denote where one dispinfo fits on another.
// Note: tables are generated based on these indices so make sure to update
//       them if these indices are changed.
typedef enum
{
	CORNER_TO_CORNER = 0,
	CORNER_TO_MIDPOINT = 1,
	MIDPOINT_TO_CORNER = 2
} NeighborSpan;


// These define relative orientations of displacement neighbors.
typedef enum
{
	ORIENTATION_CCW_0 = 0,
	ORIENTATION_CCW_90 = 1,
	ORIENTATION_CCW_180 = 2,
	ORIENTATION_CCW_270 = 3
} NeighborOrientation;

// NOTE: see the section above titled "displacement neighbor rules".
struct CDispSubNeighbor
{
public:
	unsigned short		GetNeighborIndex() const { return m_iNeighbor; }
	NeighborSpan		GetSpan() const { return (NeighborSpan)m_Span; }
	NeighborSpan		GetNeighborSpan() const { return (NeighborSpan)m_NeighborSpan; }
	NeighborOrientation	GetNeighborOrientation() const { return (NeighborOrientation)m_NeighborOrientation; }

	bool				IsValid() const { return m_iNeighbor != 0xFFFF; }
	void				SetInvalid() { m_iNeighbor = 0xFFFF; }


public:
	unsigned short		m_iNeighbor;		// This indexes into ddispinfos.
											// 0xFFFF if there is no neighbor here.

	unsigned char		m_NeighborOrientation;		// (CCW) rotation of the neighbor wrt this displacement.

													// These use the NeighborSpan type.
	unsigned char		m_Span;						// Where the neighbor fits onto this side of our displacement.
	unsigned char		m_NeighborSpan;				// Where we fit onto our neighbor.
};

// NOTE: see the section above titled "displacement neighbor rules".
class CDispNeighbor
{
public:
	void				SetInvalid() { m_SubNeighbors[0].SetInvalid(); m_SubNeighbors[1].SetInvalid(); }

	// Returns false if there isn't anything touching this edge.
	bool				IsValid() { return m_SubNeighbors[0].IsValid() || m_SubNeighbors[1].IsValid(); }


public:
	// Note: if there is a neighbor that fills the whole side (CORNER_TO_CORNER),
	//       then it will always be in CDispNeighbor::m_Neighbors[0]
	CDispSubNeighbor	m_SubNeighbors[2];
};

class CDispCornerNeighbors
{
public:
	void			SetInvalid() { m_nNeighbors = 0; }


public:
	unsigned short	m_Neighbors[MAX_DISP_CORNER_NEIGHBORS];	// indices of neighbors.
	unsigned char	m_nNeighbors;
};

class ddispinfo_t // lump 26
{
public:
	int			NumVerts() const { return NUM_DISP_POWER_VERTS(power); }
	int			NumTris() const { return NUM_DISP_POWER_TRIS(power); }

public:
	Vector		startPosition;						// start position used for orientation -- (added BSPVERSION 6)
	int			m_iDispVertStart;					// Index into LUMP_DISP_VERTS.
	int			m_iDispTriStart;					// Index into LUMP_DISP_TRIS.

	int         power;                              // power - indicates size of map (2^power + 1)
	int         minTess;                            // minimum tesselation allowed
	float       smoothingAngle;                     // lighting smoothing angle
	int         contents;                           // surface contents

	unsigned short	m_iMapFace;						// Which map face this displacement comes from.

	int			m_iLightmapAlphaStart;				// Index into ddisplightmapalpha.
													// The count is m_pParent->lightmapTextureSizeInLuxels[0]*m_pParent->lightmapTextureSizeInLuxels[1].

	int			m_iLightmapSamplePositionStart;		// Index into LUMP_DISP_LIGHTMAP_SAMPLE_POSITIONS.

	CDispNeighbor			m_EdgeNeighbors[4];		// Indexed by NEIGHBOREDGE_ defines.
	CDispCornerNeighbors	m_CornerNeighbors[4];	// Indexed by CORNER_ defines.

	enum unnamed { ALLOWEDVERTS_SIZE = PAD_NUMBER(MAX_DISPVERTS, 32) / 32 };
	unsigned long	m_AllowedVerts[ALLOWEDVERTS_SIZE];	// This is built based on the layout and sizes of our neighbors
														// and tells us which vertices are allowed to be active.
};

class CDispVert // lump 33
{
public:
	Vector		m_vVector;		// Vector field defining displacement volume.
	float		m_flDist;		// Displacement distances.
	float		m_flAlpha;		// "per vertex" alpha values.
};

struct dleafwaterdata_t // lump 36
{
	float	surfaceZ;
	float	minZ;
	short	surfaceTexInfoID;
};

struct dprimitive_t // lump 37
{
	unsigned char type;
	unsigned short	firstIndex;
	unsigned short	indexCount;
	unsigned short	firstVert;
	unsigned short	vertCount;
};

struct dprimvert_t // lump 38
{
	Vector		pos;
};

struct dcubemapsample_t // lump 42
{
	int			origin[3];			// position of light snapped to the nearest integer
									// the filename for the vtf file is derived from the position
	unsigned char size;				// 0 - default
									// otherwise, 1<<(size-1)
};

#define OVERLAY_BSP_FACE_COUNT	64

#define OVERLAY_NUM_RENDER_ORDERS		(1<<OVERLAY_RENDER_ORDER_NUM_BITS)
#define OVERLAY_RENDER_ORDER_NUM_BITS	2
#define OVERLAY_RENDER_ORDER_MASK		0xC000	// top 2 bits set

struct doverlay_t // lump 45
{
	int			nId;
	short		nTexInfo;

	// Accessors..
	void			SetFaceCount(unsigned short count);
	unsigned short	GetFaceCount() const;

	void			SetRenderOrder(unsigned short order);
	unsigned short	GetRenderOrder() const;

private:
	unsigned short	m_nFaceCountAndRenderOrder;

public:
	int			aFaces[OVERLAY_BSP_FACE_COUNT];
	float		flU[2];
	float		flV[2];
	Vector		vecUVPoints[4];
	Vector		vecOrigin;
	Vector		vecBasisNormal;
};

inline void doverlay_t::SetFaceCount(unsigned short count)
{
	m_nFaceCountAndRenderOrder &= OVERLAY_RENDER_ORDER_MASK;
	m_nFaceCountAndRenderOrder |= (count & ~OVERLAY_RENDER_ORDER_MASK);
}

inline unsigned short doverlay_t::GetFaceCount() const
{
	return m_nFaceCountAndRenderOrder & ~OVERLAY_RENDER_ORDER_MASK;
}

inline void doverlay_t::SetRenderOrder(unsigned short order)
{
	m_nFaceCountAndRenderOrder &= ~OVERLAY_RENDER_ORDER_MASK;
	m_nFaceCountAndRenderOrder |= (order << (16 - OVERLAY_RENDER_ORDER_NUM_BITS));	// leave 2 bits for render order.
}

inline unsigned short doverlay_t::GetRenderOrder() const
{
	return (m_nFaceCountAndRenderOrder >> (16 - OVERLAY_RENDER_ORDER_NUM_BITS));
}

class CFaceMacroTextureInfo // lump 47
{
public:
	// This looks up into g_TexDataStringTable, which looks up into g_TexDataStringData.
	// 0xFFFF if the face has no macro texture.
	unsigned short m_MacroTextureNameID;
};

class CDispTri // lump 48
{
public:
	unsigned short m_uiTags;		// Displacement triangle tags.
};

#define WATEROVERLAY_BSP_FACE_COUNT				256
#define WATEROVERLAY_RENDER_ORDER_NUM_BITS		2
#define WATEROVERLAY_NUM_RENDER_ORDERS			(1<<WATEROVERLAY_RENDER_ORDER_NUM_BITS)
#define WATEROVERLAY_RENDER_ORDER_MASK			0xC000	// top 2 bits set

struct dwateroverlay_t // lump 50
{
	int				nId;
	short			nTexInfo;

	// Accessors..
	void			SetFaceCount(unsigned short count);
	unsigned short	GetFaceCount() const;
	void			SetRenderOrder(unsigned short order);
	unsigned short	GetRenderOrder() const;

private:

	unsigned short	m_nFaceCountAndRenderOrder;

public:

	int				aFaces[WATEROVERLAY_BSP_FACE_COUNT];
	float			flU[2];
	float			flV[2];
	Vector			vecUVPoints[4];
	Vector			vecOrigin;
	Vector			vecBasisNormal;
};

inline void dwateroverlay_t::SetFaceCount(unsigned short count)
{
	m_nFaceCountAndRenderOrder &= WATEROVERLAY_RENDER_ORDER_MASK;
	m_nFaceCountAndRenderOrder |= (count & ~WATEROVERLAY_RENDER_ORDER_MASK);
}

inline unsigned short dwateroverlay_t::GetFaceCount() const
{
	return m_nFaceCountAndRenderOrder & ~WATEROVERLAY_RENDER_ORDER_MASK;
}

inline void dwateroverlay_t::SetRenderOrder(unsigned short order)
{
	m_nFaceCountAndRenderOrder &= ~WATEROVERLAY_RENDER_ORDER_MASK;
	m_nFaceCountAndRenderOrder |= (order << (16 - WATEROVERLAY_RENDER_ORDER_NUM_BITS));	// leave 2 bits for render order.
}

inline unsigned short dwateroverlay_t::GetRenderOrder() const
{
	return (m_nFaceCountAndRenderOrder >> (16 - WATEROVERLAY_RENDER_ORDER_NUM_BITS));
}

struct dleafambientindex_t
{
	unsigned short ambientSampleCount;
	unsigned short firstAmbientSample;
};

// compressed color format 
struct ColorRGBExp32
{
	byte r, g, b;
	signed char exponent;
};

struct CompressedLightCube
{
	ColorRGBExp32 m_Color[6];
};

struct dleafambientlighting_t
{
	CompressedLightCube	cube;
	byte x;		// fixed point fraction of leaf bounds
	byte y;		// fixed point fraction of leaf bounds
	byte z;		// fixed point fraction of leaf bounds
	byte pad;	// unused
};

struct doverlayfade_t // lump 60
{
	float flFadeDistMinSq;
	float flFadeDistMaxSq;
};

struct plane_t : public dplane_t
{
	plane_t			*hash_chain;

	plane_t() { normal.Init(); }
};

struct epair_t
{
	epair_t	*next;
	char	*key;
	char	*value;
};

struct portal_t;

struct entity_t
{
	Vector		origin;
	int			firstbrush;
	int			numbrushes;
	epair_t		*epairs;

	// only valid for func_areaportals
	int			areaportalnum;
	int			portalareas[2];
	portal_t	*m_pPortalsLeadingIntoAreas[2];	// portals leading into portalareas
};

#endif // _GPUA_BSPFILE_H_