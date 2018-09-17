#ifndef _GPUARAD_TRACE_H_
#define _GPUARAD_TRACE_H_

#include <vector>

#include "timer.h"
#include "vmatrix.h"

#include "raytrace.h"

#include "bsp.h"
#include "poly.h"
#include "bspflags.h"

// Raytracing
#define TRACE_ID_SKY           0x01000000  // sky face ray blocker
#define TRACE_ID_OPAQUE        0x02000000  // everyday light blocking face
#define TRACE_ID_STATICPROP    0x04000000  // static prop - lower bits are prop ID

class CTrace : public CBSP
{
public:
	CTrace(const char* filename);

	void AddBrushesForRayTrace();
	void GetBrushesRecursively(int node, std::vector<int> &list);
	void AddBrushToRaytraceEnvironment(dbrush_t* brush, VMatrix &identity);

	dface_t *m_pFaces;
};

#endif // _GPUARAD_TRACE_H_