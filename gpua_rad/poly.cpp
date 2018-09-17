#include "poly.h"

// doesn't seem to need to be here? -- in threads.h
//extern int numthreads;

// counters are only bumped when running single threaded,
// because they are an awefull coherence problem
int	c_active_windings;
int	c_peak_windings;
int	c_winding_allocs;
int	c_winding_points;

winding_t *winding_pool[MAX_POINTS_ON_WINDING + 4];

winding_t *AllocWinding(int points)
{
	winding_t	*w;

	c_winding_allocs++;
	c_winding_points += points;
	c_active_windings++;
	if (c_active_windings > c_peak_windings)
		c_peak_windings = c_active_windings;

	if (winding_pool[points])
	{
		w = winding_pool[points];
		winding_pool[points] = w->next;
	}
	else
	{
		w = (winding_t *)malloc(sizeof(*w));
		w->p = (Vector *)calloc(points, sizeof(Vector));
	}

	w->numpoints = 0; // None are occupied yet even though allocated.
	w->maxpoints = points;
	w->next = NULL;
	return w;
}

void FreeWinding(winding_t *w)
{
	if (w->numpoints == 0xdeaddead)
		pp::Error("FreeWinding: freed a freed winding");

	w->numpoints = 0xdeaddead; // flag as freed
	w->next = winding_pool[w->maxpoints];
	winding_pool[w->maxpoints] = w;
}

/*
=================
BaseWindingForPlane
=================
*/
winding_t *BaseWindingForPlane(Vector &normal, float dist)
{
	int		i, x;
	float	max, v;
	Vector	org, vright, vup;
	winding_t	*w;

	// find the major axis

	max = -1;
	x = -1;
	for (i = 0; i<3; i++)
	{
		v = fabs(normal[i]);
		if (v > max)
		{
			x = i;
			max = v;
		}
	}

	if (x == -1)
		pp::Error("BaseWindingForPlane: no axis found");

	vup.Init();

	switch (x)
	{
	case 0:
	case 1:
		vup[2] = 1;
		break;
	case 2:
		vup[0] = 1;
		break;
	}

	v = DotProduct(vup, normal);
	VectorMA(vup, -v, normal, vup);
	VectorNormalize(vup);

	VectorScale(normal, dist, org);

	CrossProduct(vup, normal, vright);

	VectorScale(vup, (MAX_COORD_INTEGER * 4), vup);
	VectorScale(vright, (MAX_COORD_INTEGER * 4), vright);

	// project a really big	axis aligned box onto the plane
	w = AllocWinding(4);

	VectorSubtract(org, vright, w->p[0]);
	VectorAdd(w->p[0], vup, w->p[0]);

	VectorAdd(org, vright, w->p[1]);
	VectorAdd(w->p[1], vup, w->p[1]);

	VectorAdd(org, vright, w->p[2]);
	VectorSubtract(w->p[2], vup, w->p[2]);

	VectorSubtract(org, vright, w->p[3]);
	VectorSubtract(w->p[3], vup, w->p[3]);

	w->numpoints = 4;

	return w;
}

/*
=============
ChopWindingInPlace
=============
*/
void ChopWindingInPlace(winding_t **inout, Vector &normal, float dist, float epsilon)
{
	winding_t	*in;
	float	dists[MAX_POINTS_ON_WINDING + 4];
	int		sides[MAX_POINTS_ON_WINDING + 4];
	int		counts[3];
	float	dot;
	int		i, j;
	Vector	mid(0.0f, 0.0f, 0.0f);
	winding_t	*f;
	int		maxpts;

	in = *inout;
	counts[0] = counts[1] = counts[2] = 0;
	// determine sides for each point
	for (i = 0; i<in->numpoints; i++)
	{
		dot = DotProduct(in->p[i], normal);
		dot -= dist;
		dists[i] = dot;
		if (dot > epsilon)
		{
			sides[i] = SIDE_FRONT;
		}
		else if (dot < -epsilon)
		{
			sides[i] = SIDE_BACK;
		}
		else
		{
			sides[i] = SIDE_ON;
		}
		counts[sides[i]]++;
	}
	sides[i] = sides[0];
	dists[i] = dists[0];

	if (!counts[0])
	{
		FreeWinding(in);
		*inout = NULL;
		return;
	}
	if (!counts[1])
		return;		// inout stays the same

	maxpts = in->numpoints + 4;	// cant use counts[0]+2 because
								// of fp grouping errors

	f = AllocWinding(maxpts);

	for (i = 0; i<in->numpoints; i++)
	{
		Vector& p1 = in->p[i];

		if (sides[i] == SIDE_ON)
		{
			VectorCopy(p1, f->p[f->numpoints]);
			f->numpoints++;
			continue;
		}

		if (sides[i] == SIDE_FRONT)
		{
			VectorCopy(p1, f->p[f->numpoints]);
			f->numpoints++;
		}

		if (sides[i + 1] == SIDE_ON || sides[i + 1] == sides[i])
			continue;

		// generate a split point
		Vector& p2 = in->p[(i + 1) % in->numpoints];

		dot = dists[i] / (dists[i] - dists[i + 1]);
		for (j = 0; j<3; j++)
		{	// avoid round off error when possible
			if (normal[j] == 1)
				mid[j] = dist;
			else if (normal[j] == -1)
				mid[j] = -dist;
			else
				mid[j] = p1[j] + dot*(p2[j] - p1[j]);
		}

		VectorCopy(mid, f->p[f->numpoints]);
		f->numpoints++;
	}

	if (f->numpoints > maxpts)
		pp::Error("ClipWinding: points exceeded estimate");
	if (f->numpoints > MAX_POINTS_ON_WINDING)
		pp::Error("ClipWinding: MAX_POINTS_ON_WINDING");

	FreeWinding(in);
	*inout = f;
}