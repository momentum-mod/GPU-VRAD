#ifndef _GPUARAD_POLY_H_
#define _GPUARAD_POLY_H_

#include "prettyprint.h"

#include "worldsize.h"
#include "vector.h"

#define	MAX_POINTS_ON_WINDING	64

// you can define on_epsilon in the makefile as tighter
// point on plane side epsilon
// todo: need a world-space epsilon, a lightmap-space epsilon, and a texture space epsilon
// or at least convert from a world-space epsilon to lightmap and texture space epsilons
#ifndef	ON_EPSILON
#define	ON_EPSILON	0.1
#endif

// Used to represent sides of things like planes.
#define	SIDE_FRONT	0
#define	SIDE_BACK	1
#define	SIDE_ON		2
#define SIDE_CROSS  -2      // necessary for polylib.c

class winding_t
{
public:
	int		numpoints;
	Vector	*p;		// variable sized
	int		maxpoints;
	winding_t *next;
};

void ChopWindingInPlace(winding_t **inout, Vector &normal, float dist, float epsilon);
winding_t *BaseWindingForPlane(Vector &normal, float dist);

winding_t *AllocWinding(int points);
void FreeWinding(winding_t *w);

#endif // _GPUARAD_POLY_H_