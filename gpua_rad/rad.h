#ifndef _GPUA_RAD_H_
#define _GPUA_RAD_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "prettyprint.h"

#include "trace.h"

#define INVALID_INDEX -1

struct texlight_t
{
	char			name[MAX_TEXLIGHT_NAME];
	Vector			value;
	const char*		filename;
};

class CRAD : public CTrace
{
public:
	CRAD(const char* filename);
	~CRAD();
	void ReadLights(const char* filename);
	int LightForString(const char* light, Vector &intensity);
	size_t GetNextColumn(std::string s, const char* delims, size_t start);
	void Setup();
	int CountClusters();

private:
	// Settings
	bool m_bHDR;
	Vector m_vecAmbient;
	float m_flLightscale;
	bool m_bStaticPropLighting;
	unsigned int m_iNumBounce; // 25; /* Originally this was 8 */
	// Important stuff

	/* TODO SetHdrMode() */
	std::vector<uint8_t>* m_pvLightData;
	std::vector<dleafambientindex_t>* m_pvLeafAmbientIndex;
	std::vector<dleafambientlighting_t>* m_pvLeafAmbientLighting;
	/*
	int *pNumWorldLights = &numworldlightsLDR;
	dworldlight_t *m_pdWorldLights = dworldlightsLDR;
	*/
	/* TODO SetHdrMode() */

	std::vector<int> m_vFacePatches;
	std::vector<int> m_vFaceParents;
	std::vector<int> m_vClusterChildren;

	std::vector<texlight_t> m_vTexLights;
	std::vector<const char*> m_vSzNoShadowMaterialNames;
	std::vector<const char*> m_vSzForcedTextureShadowsOnModel;
};
#endif