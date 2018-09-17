#include "vector.h"

Vector::Vector()
{
	Init();
}

inline void Vector::Init(float ix, float iy, float iz)
{
	x = ix;
	y = iy;
	z = iz;
}

void Vector::dump()
{
	pp::Info("Vector(%f, %f, %f)", x, y, z);
}

Vector Vector::operator+(const Vector & v) const
{
	Vector res;
	VectorAdd(*this, v, res);
	return res;
}

Vector Vector::operator-(const Vector & v) const
{
	Vector res;
	VectorSubtract(*this, v, res);
	return res;
}

Vector Vector::operator*(const Vector & v) const
{
	Vector res;
	VectorMultiply(*this, v, res);
	return res;
}

Vector Vector::operator/(const Vector & v) const
{
	Vector res;
	VectorDivide(*this, v, res);
	return res;
}

Vector Vector::operator*(float fl) const
{
	Vector res;
	VectorMultiply(*this, fl, res);
	return res;
}

Vector Vector::operator/(float fl) const
{
	Vector res;
	VectorDivide(*this, fl, res);
	return res;
}

Vector Vector::Cross(const Vector & vOther) const
{
	Vector res;
	CrossProduct(*this, vOther, res);
	return res;
}

float Vector::NormalizeInPlace()
{
	return VectorNormalize(*this);
}

float Vector::Dot(const Vector & vOther) const
{
	return DotProduct(*this, vOther);
}
