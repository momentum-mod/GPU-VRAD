#ifndef _GPUA_MATH_VECTOR_H_
#define _GPUA_MATH_VECTOR_H_

#include <fstream>

#include "prettyprint.h"
#include <emmintrin.h>

class VectorByValue;

class Vector
{
public:
	float x, y, z;

	// Construction/destruction:
	Vector();
	Vector(float ix, float iy, float iz)
	{
		x = ix;
		y = iy;
		z = iz;
	}

	void Init(float ix = 0.f, float iy = 0.f, float iz = 0.f);

	void dump();
	
	// operators
	bool operator==(const Vector& src) const
	{
		return (src.x == x) && (src.y == y) && (src.z == z);
	}
	bool operator!=(const Vector& src) const
	{
		return (src.x != x) || (src.y != y) || (src.z != z);
	}

	// arithmetic operations
	FORCEINLINE Vector&	operator+=(const Vector &v)
	{
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	FORCEINLINE Vector&	operator-=(const Vector &v)
	{
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	FORCEINLINE Vector&	operator*=(const Vector &v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	FORCEINLINE Vector&	operator*=(float fl)
	{
		x *= fl;
		y *= fl;
		z *= fl;
		return *this;
	}

	FORCEINLINE Vector&	operator/=(const Vector &v)
	{
		if (v.x == 0.0f || v.y == 0.0f || v.z == 0.0f)
			return *this;
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	FORCEINLINE Vector&	operator/=(float fl)
	{
		if(fl == 0.0f)
			return *this;
		float oofl = 1.0f / fl;
		x *= oofl;
		y *= oofl;
		z *= oofl;
		return *this;
	}

	FORCEINLINE Vector&	operator+=(float fl)
	{
		x += fl;
		y += fl;
		z += fl;
		return *this;
	}

	FORCEINLINE Vector&	operator-=(float fl)
	{
		x -= fl;
		y -= fl;
		z -= fl;
		return *this;
	}

	Vector& operator=(const Vector &vOther)
	{
		x = vOther.x; y = vOther.y; z = vOther.z;
		return *this;
	}

	Vector	operator-(void) const
	{
		return Vector(-x, -y, -z);
	}

	Vector	operator+(const Vector& v) const;
	Vector	operator-(const Vector& v) const;
	Vector	operator*(const Vector& v) const;
	Vector	operator/(const Vector& v) const;
	Vector	operator*(float fl) const;
	Vector	operator/(float fl) const;

	FORCEINLINE float& operator[](int i) const
	{
		return ((float*)this)[i];
	}

	operator VectorByValue &() { return *((VectorByValue *)(this)); }
	operator const VectorByValue &() const { return *((const VectorByValue *)(this)); }

	FORCEINLINE float LengthSqr(void) const
	{
		return (x*x + y*y + z*z);
	}

	Vector Cross(const Vector& vOther) const;

	float NormalizeInPlace();

	// for backwards compatability
	float Dot(const Vector& vOther) const;
};

FORCEINLINE float DotProduct(const Vector& a, const Vector& b)
{
	return(a.x*b.x + a.y*b.y + a.z*b.z);
}

FORCEINLINE void VectorCopy(const Vector& src, Vector& dst)
{
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

FORCEINLINE void VectorSubtract(const Vector& a, const Vector& b, Vector& c)
{
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}

FORCEINLINE void VectorAdd(const Vector& a, const Vector& b, Vector& c)
{
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
}

FORCEINLINE void VectorDivide(const Vector& a, float b, Vector& c)
{
	float oob = 1.0f / b;
	c.x = a.x * oob;
	c.y = a.y * oob;
	c.z = a.z * oob;
}

FORCEINLINE void VectorDivide(const Vector& a, const Vector& b, Vector& c)
{
	c.x = a.x / b.x;
	c.y = a.y / b.y;
	c.z = a.z / b.z;
}

FORCEINLINE void VectorMultiply(const Vector& a, float b, Vector& c)
{
	c.x = a.x * b;
	c.y = a.y * b;
	c.z = a.z * b;
}

FORCEINLINE void VectorMultiply(const Vector& a, const Vector& b, Vector& c)
{
	c.x = a.x * b.x;
	c.y = a.y * b.y;
	c.z = a.z * b.z;
}

inline void VectorScale(const Vector& in, float scale, Vector& result)
{
	VectorMultiply(in, scale, result);
}

inline void VectorScale(const Vector& in, Vector &scale, Vector& result)
{
	VectorMultiply(in, scale, result);
}

FORCEINLINE void VectorMAInline(const float* start, float scale, const float* direction, float* dest)
{
	dest[0] = start[0] + direction[0] * scale;
	dest[1] = start[1] + direction[1] * scale;
	dest[2] = start[2] + direction[2] * scale;
}

FORCEINLINE void VectorMAInline(const Vector& start, float scale, const Vector& direction, Vector& dest)
{
	dest.x = start.x + direction.x*scale;
	dest.y = start.y + direction.y*scale;
	dest.z = start.z + direction.z*scale;
}

FORCEINLINE void VectorMA(const Vector& start, float scale, const Vector& direction, Vector& dest)
{
	VectorMAInline(start, scale, direction, dest);
}

FORCEINLINE void VectorMA(const float * start, float scale, const float *direction, float *dest)
{
	VectorMAInline(start, scale, direction, dest);
}

#if defined(__i386__) || defined(_M_IX86)
inline void _SSE_RSqrtInline(float a, float* out)
{
	__m128  xx = _mm_load_ss(&a);
	__m128  xr = _mm_rsqrt_ss(xx);
	__m128  xt;
	xt = _mm_mul_ss(xr, xr);
	xt = _mm_mul_ss(xt, xx);
	xt = _mm_sub_ss(_mm_set_ss(3.f), xt);
	xt = _mm_mul_ss(xt, _mm_set_ss(0.5f));
	xr = _mm_mul_ss(xr, xt);
	_mm_store_ss(out, xr);
}
#endif

FORCEINLINE float VectorNormalize(Vector& vec)
{
#ifndef DEBUG // stop crashing my edit-and-continue!
#if defined(__i386__) || defined(_M_IX86)
#define DO_SSE_OPTIMIZATION
#endif
#endif

#if defined( DO_SSE_OPTIMIZATION )
	float sqrlen = vec.LengthSqr() + 1.0e-10f, invlen;
	_SSE_RSqrtInline(sqrlen, &invlen);
	vec.x *= invlen;
	vec.y *= invlen;
	vec.z *= invlen;
	return sqrlen * invlen;
#else
	extern float (FASTCALL *pfVectorNormalize)(Vector& v);
	return (*pfVectorNormalize)(vec);
#endif
}

inline void CrossProduct(const Vector& a, const Vector& b, Vector& result)
{
	result.x = a.y*b.z - a.z*b.y;
	result.y = a.z*b.x - a.x*b.z;
	result.z = a.x*b.y - a.y*b.x;
}

//-----------------------------------------------------------------------------
// Allows us to specifically pass the vector by value when we need to
//-----------------------------------------------------------------------------
class VectorByValue : public Vector
{
public:
	// Construction/destruction:
	VectorByValue(void) : Vector() {}
	VectorByValue(float X, float Y, float Z) : Vector(X, Y, Z) {}
	VectorByValue(const VectorByValue& vOther) { *this = vOther; }
};

#endif // _GPUA_MATH_VECTOR_H_