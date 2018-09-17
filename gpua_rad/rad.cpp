#include "rad.h"

CRAD::CRAD(const char * filename) : CTrace(filename)
{
	m_bHDR = true;
	m_iNumBounce = 100;
	m_vecAmbient.Init();
	m_flLightscale = 1.0f;
	m_bStaticPropLighting = false;
}

CRAD::~CRAD()
{
	m_vTexLights.clear();
	m_vSzNoShadowMaterialNames.clear();
	m_vSzForcedTextureShadowsOnModel.clear();
}

void CRAD::ReadLights(const char * filename)
{
	Vector v;
	texlight_t tl;

	std::string s;
	std::ifstream ifs(filename);

	char sztl[MAX_TEXLIGHT_NAME];
	int file_texlights = 0;

	if (!ifs)
	{
		pp::Warning("Couldn't open texlight file %s.", filename);
	}

	pp::Info("Reading texlights from %s", filename);

	while (std::getline(ifs, s))
	{
		size_t start_off;
		size_t off;

		// empty line
		if (!s.length())
		{
			continue;
		}

		if (s.rfind("hdr:", 0) == 0)
		{
			start_off = 5;
			if (!m_bHDR)
			{
				continue;
			}
		}
		else if (s.rfind("ldr:", 0) == 0)
		{
			start_off = 5;
			if (m_bHDR)
			{
				continue;
			}
		}
		else if (s.rfind("noshadow ", 0) == 0)
		{
			off = GetNextColumn(s, " \t", start_off);

			if (off < 0 && off != std::string::npos)
			{
				pp::Warning("Could not find delimeter");
				continue;
			}

			off++;

			m_vSzNoShadowMaterialNames.push_back(_strdup(s.c_str() + off));
			//pp::Info("Found noshadow: %s", s.c_str() + off);
			continue;
		}
		else if (s.rfind("forcetextureshadow ", 0) == 0)
		{
			off = GetNextColumn(s, " \t", start_off);

			if (off < 0 && off != std::string::npos)
			{
				pp::Warning("Could not find delimeter");
				continue;
			}

			off++;

			m_vSzForcedTextureShadowsOnModel.push_back(_strdup(s.c_str() + off));
			//pp::Info("Found forced shadow: %s", s.c_str() + off);
			continue;
		}

		off = GetNextColumn(s, " \t", start_off);

		if (off < 0 && off != std::string::npos)
		{
			pp::Warning("Could not find delimeter");
			continue;
		}

		if (!LightForString(s.c_str() + off, v))
		{
			continue;
		}

		if (m_vTexLights.size() > MAX_TEXLIGHTS)
		{
			pp::Error("Too many texlights, max = %d", MAX_TEXLIGHTS);
		}

		size_t len = ((off - start_off) >= MAX_TEXLIGHT_NAME) ? (MAX_TEXLIGHT_NAME - 1) : (off - start_off);
		strncpy_s(sztl, s.c_str() + start_off, len);
		sztl[len] = '\0';

		for (auto texlight : m_vTexLights)
		{
			if (strcmp(texlight.name, sztl) == 0)
			{
				if (strcmp(texlight.filename, filename) == 0)
				{
					pp::Error("ERROR\a: Duplication of '%s' in file '%s'!\n",
						texlight.name, texlight.filename);
				}
				else if (texlight.value[0] != v[0]
					|| texlight.value[1] != v[1]
					|| texlight.value[2] != v[2])
				{
					pp::Warning("Warning: Overriding '%s' from '%s' with '%s'!\n",
						texlight.name, texlight.filename, filename);
				}
				else
				{
					pp::Warning("Warning: Redundant '%s' def in '%s' AND '%s'!\n",
						texlight.name, texlight.filename, filename);
				}
				break;
			}
		}

		strncpy_s(tl.name, sztl, MAX_TEXLIGHT_NAME);
		tl.value = v;
		tl.filename = filename;
		file_texlights++;
		m_vTexLights.push_back(tl);
		//pp::Info("Found texlight: %s, %f, %f, %f", tl.name, tl.value.x, tl.value.y, tl.value.z);
	}

	pp::Success("Parsed %i texlights from %s\n", file_texlights, filename);

	ifs.close();
}

int CRAD::LightForString(const char * light, Vector & intensity)
{
	double r, g, b, scaler;
	int argCnt;

	intensity.Init();

	// scanf into doubles, then assign, so it is vec_t size independent
	r = g = b = scaler = 0;
	double r_hdr, g_hdr, b_hdr, scaler_hdr;
	argCnt = sscanf_s(light, "%lf %lf %lf %lf %lf %lf %lf %lf",
		&r, &g, &b, &scaler, &r_hdr, &g_hdr, &b_hdr, &scaler_hdr);

	if (argCnt == 8) 											// 2 4-tuples
	{
		if (m_bHDR)
		{
			r = r_hdr;
			g = g_hdr;
			b = b_hdr;
			scaler = scaler_hdr;
		}
		argCnt = 4;
	}

	// make sure light is legal
	if (r < 0.0f || g < 0.0f || b < 0.0f || scaler < 0.0f)
	{
		pp::Warning("Invalid light\n");
		intensity.Init(0.0f, 0.0f, 0.0f);
		return false;
	}

	intensity[0] = float(pow(r / 255.0, 2.2) * 255.0); // convert to linear

	switch (argCnt)
	{
	case 1:
		// The R,G,B values are all equal.
		intensity[1] = intensity[2] = intensity[0];
		break;

	case 3:
	case 4:
		// Save the other two G,B values.
		intensity[1] = float(pow(g / 255.0, 2.2) * 255);
		intensity[2] = float(pow(b / 255.0, 2.2) * 255);

		// Did we also get an "intensity" scaler value too?
		if (argCnt == 4)
		{
			// Scale the normalized 0-255 R,G,B values by the intensity scaler
			VectorScale(intensity, float(scaler / 255.0f), intensity);
		}
		break;

	default:
		pp::Warning("Unknown light specifier type - %s\n", light);
		return false;
	}
	// scale up source lights by scaling factor
	VectorScale(intensity, m_flLightscale, intensity);
	return true;
}

size_t CRAD::GetNextColumn(std::string s, const char * delims, size_t start)
{
	size_t tmp_off = start;
	size_t off;

	off = s.find_first_of(" \t", tmp_off);

	while (off != std::string::npos && (off - tmp_off) < 0)
	{
		tmp_off = off + 1;
		off = s.find_first_of(" \t", tmp_off);
	}

	return off;
}

void CRAD::Setup()
{
	if (m_bStaticPropLighting)
	{
		m_fLevelFlags |= m_bHDR ? LVLFLAGS_BAKED_STATIC_PROP_LIGHTING_HDR : LVLFLAGS_BAKED_STATIC_PROP_LIGHTING_NONHDR;
	}
	else
	{
		m_fLevelFlags &= ~(LVLFLAGS_BAKED_STATIC_PROP_LIGHTING_HDR | LVLFLAGS_BAKED_STATIC_PROP_LIGHTING_NONHDR);
	}

	// now, we need to set our face ptr depending upon hdr, and if hdr, init it
	if (m_bHDR)
	{
		m_pFaces = m_dFacesHDR;
		if (m_iNumFacesHDR == 0)
		{
			m_iNumFacesHDR = m_iNumFaces;
			memcpy(m_dFacesHDR, m_dFaces, m_iNumFaces * sizeof(*m_dFaces));
		}
	}
	else
	{
		m_pFaces = m_dFaces;
	}

	if (!m_iVisDataSize)
	{
		pp::Info("No vis information, direct lighting only.\n");
		m_iNumBounce = 0;
		m_vecAmbient[0] = m_vecAmbient[1] = m_vecAmbient[2] = 0.1f;
		m_dVis->numclusters = CountClusters();
	}

	m_vFacePatches.resize(MAX_MAP_FACES);
	m_vFaceParents.resize(MAX_MAP_FACES);
	m_vClusterChildren.resize(MAX_MAP_CLUSTERS);

	for (int i = 0; i < MAX_MAP_FACES; i++)
	{
		m_vFacePatches[i] = INVALID_INDEX;
		m_vFaceParents[i] = INVALID_INDEX;
	}

	for (int i = 0; i < MAX_MAP_CLUSTERS; i++)
	{
		m_vClusterChildren[i] = INVALID_INDEX;
	}
}

int CRAD::CountClusters()
{
	int cluster_count = 0;

	for (int i = 0; i < m_iNumLeafs; i++)
	{
		if (m_dLeafs[i].cluster > cluster_count)
			cluster_count = m_dLeafs[i].cluster;
	}

	return cluster_count + 1;
}
