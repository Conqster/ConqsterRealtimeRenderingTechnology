#pragma once

#pragma once
#include <assert.h>
#include <math.h>

/// <summary>
/// Vector 2 Maths Libary 
/// </summary>
class Vector2
{
public:
	Vector2();

	float x;
	float y;

	Vector2(const float value);
	Vector2(const Vector2& rhs);
	Vector2(float X, float Y);
	Vector2(const float* xy);

	Vector2& operator =(const Vector2& rhs);
	bool operator == (const Vector2& rhs) const;
	bool operator != (const Vector2& rhs) const;

	Vector2 operator +(const Vector2& rhs) const;
	Vector2 operator - (const Vector2& rhs) const;
	Vector2 operator * (const float rhs) const;
	const Vector2& operator +=(const Vector2& rhs);
	const Vector2& operator -=(const Vector2& rhs);
	const Vector2& operator *= (const float rhs);
	const Vector2& operator /= (const float rhs);

	float operator [] (const int idx) const;
	float& operator [] (const int idx);

	float GetMagnitude() const;
	const Vector2& Normalize();
	bool IsValid() const;
	float Dot(const Vector2& rhs) const
	{
		return x * rhs.x + y * rhs.y;
	}

	const float* ToPtr() const
	{
		return &x;
	}

};


inline Vector2::Vector2() : x(0), y(0) {

}

inline Vector2::Vector2(const float value) : x(value), y(value) {

}

inline Vector2::Vector2(const Vector2& rhs) : x(rhs.x), y(rhs.y) {

}

inline Vector2::Vector2(float X, float Y) : x(X), y(Y) {

}

inline Vector2::Vector2(const float* xy) : x(xy[0]), y(xy[1]) {

}

inline Vector2& Vector2::operator=(const Vector2& rhs)
{
	x = rhs.x;
	y = rhs.y;
	return *this;
}

inline bool Vector2::operator==(const Vector2& rhs) const
{
	if (x != rhs.x)
		return false;

	if (y != rhs.y)
		return false;

	return true;
}


inline bool Vector2::operator != (const Vector2& rhs) const
{
	if (*this == rhs)
		return false;

	return true;
}

inline Vector2 Vector2::operator +(const Vector2& rhs) const
{
	Vector2 tempVector;
	tempVector.x = x + rhs.x;
	tempVector.y = y + rhs.y;
	return tempVector;
}

inline Vector2 Vector2::operator - (const Vector2& rhs) const
{
	Vector2 tempVector;
	tempVector.x = x - rhs.x;
	tempVector.y = y - rhs.y;
	return tempVector;
}


inline Vector2 Vector2::operator * (const float rhs) const
{
	Vector2 tempVector;
	tempVector.x = x * rhs;
	tempVector.y = y * rhs;
	return tempVector;
}

inline const Vector2& Vector2::operator +=(const Vector2& rhs)
{
	x += rhs.x;
	y += rhs.y;
	return *this;
}

inline const Vector2& Vector2::operator-=(const Vector2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}


inline const Vector2& Vector2::operator *= (const float rhs)
{
	x *= rhs;
	y *= rhs;
	return *this;
}

inline const Vector2& Vector2::operator /=(const float rhs)
{
	x /= rhs;
	y /= rhs;
	return *this;
}

inline float Vector2::operator [] (const int idx) const
{
	assert(idx >= 0 && idx < 2);
	return (&x)[idx];
}

inline float& Vector2::operator [] (const int idx)
{
	assert(idx >= 0 && idx < 2);
	return (&x)[idx];
}


inline const Vector2& Vector2::Normalize()
{
	float mag = GetMagnitude();
	float inverseMag = 1.0f / mag;


	//to check if the value inverseMag is not eqaul to infinity 
	if (0.0f * inverseMag == 0.0f * inverseMag)
	{
		x = x * inverseMag;
		y = y * inverseMag;
	}

	return *this;
}


inline float Vector2::GetMagnitude() const
{
	float mag;

	mag = x * x + y * y;
	mag = sqrtf(mag);

	return mag;
}

inline bool Vector2::IsValid() const
{
	if (x * 0.0f != x * 0.0f)
		return false;

	if (y * 0.0f != y * 0.0f)
		return false;

	return true;
}




/// <summary>
///  Vector 3 Maths Libary 
/// </summary>
class Vector3
{
public:
	Vector3();

	float x;
	float y;
	float z;

	Vector3(float value);
	Vector3(const Vector3& rhs);
	Vector3(float X, float Y, float Z);
	Vector3(const float* xyz);
	Vector3& operator = (const Vector3& rhs);
	Vector3& operator = (const float* rhs);

	bool operator == (const Vector3& rhs) const;
	bool operator != (const Vector3& rhs) const;

	Vector3 operator + (const Vector3& rhs) const;
	Vector3 operator - (const Vector3& rhs) const;
	Vector3 operator * (const float rhs) const;
	Vector3 operator /(const float rhs) const;
	const Vector3& operator +=(const Vector3& rhs);
	const Vector3& operator -=(const Vector3& rhs);
	const Vector3& operator *=(const float rhs);
	const Vector3& operator /=(const float rhs);

	float operator[] (const int idx) const;
	float& operator[] (const int idx);

	void Zero()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	Vector3 Cross(const Vector3& rhs)const;
	float Dot(const Vector3& rhs) const;

	float GetMagnitude() const;
	const Vector3& Normalize();
	float GetLenghtSqr() const
	{
		return Dot(*this);
	}
	bool IsValid() const;
	void GetOrtho(Vector3& u, Vector3& v) const;

	const float* ToPtr() const
	{
		return &x;
	}


};


inline Vector3::Vector3() : x(0), y(0), z(0) {

}

inline Vector3::Vector3(float value) : x(value), y(value), z(value) {

}

inline Vector3::Vector3(const Vector3& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {

}

inline Vector3::Vector3(float X, float Y, float Z) : x(X), y(Y), z(Z) {

}

inline Vector3::Vector3(const float* xyz) : x(xyz[0]), y(xyz[1]), z(xyz[2]) {

}

inline Vector3& Vector3::operator =(const Vector3& rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	return *this;
}

inline Vector3& Vector3::operator = (const float* rhs)
{
	x = rhs[0];
	y = rhs[1];
	z = rhs[2];
	return *this;
}

inline bool Vector3::operator == (const Vector3& rhs) const
{
	if (x != rhs.x)
		return false;

	if (y != rhs.y)
		return false;

	if (z != rhs.z)
		return false;

	return true;
}


inline bool Vector3::operator != (const Vector3& rhs) const
{
	if (*this == rhs)
		return false;

	return true;
}

inline Vector3 Vector3::operator + (const Vector3& rhs) const
{
	Vector3 tempVector;

	tempVector.x = x + rhs.x;
	tempVector.y = y + rhs.y;
	tempVector.z = z + rhs.z;

	return tempVector;
}


inline Vector3 Vector3::operator - (const Vector3& rhs) const
{
	Vector3 tempVector;

	tempVector.x = x - rhs.x;
	tempVector.y = y - rhs.y;
	tempVector.z = z - rhs.z;

	return tempVector;
}

inline Vector3 Vector3::operator * (const float rhs) const
{
	Vector3 tempVector;

	tempVector.x = x * rhs;
	tempVector.y = y * rhs;
	tempVector.z = z * rhs;

	return tempVector;
}

inline Vector3 Vector3::operator / (const float rhs) const
{
	Vector3 tempVector;

	tempVector.x = x / rhs;
	tempVector.y = y / rhs;
	tempVector.z = z / rhs;

	return tempVector;
}


inline const Vector3& Vector3::operator +=(const Vector3& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}


inline const Vector3& Vector3::operator -=(const Vector3& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}

inline const Vector3& Vector3::operator *=(const float rhs)
{
	x = x * rhs;
	y = y * rhs;
	z = z * rhs;
	return *this;
}

inline const Vector3& Vector3::operator /=(const float rhs)
{
	x = x / rhs;
	y = y / rhs;
	z = z / rhs;
	return *this;
}

inline float Vector3::operator[] (const int idx) const
{
	assert(idx >= 0 && idx < 3);
	return(&x)[idx];
}

inline float& Vector3::operator[] (const int idx)
{
	assert(idx >= 0 && idx < 3);
	return(&x)[idx];
}

inline Vector3 Vector3::Cross(const Vector3& rhs) const
{
	//The cross product is A x B, where this is A and rhs is B
	Vector3 tempVector3;
	tempVector3.x = (y * rhs.z) - (rhs.y * z);
	tempVector3.y = (rhs.x * z) - (x * rhs.z);
	tempVector3.z = (x * rhs.y) - (rhs.x * y);

	return tempVector3;
}

inline float Vector3::Dot(const Vector3& rhs) const
{
	return (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
}

inline float Vector3::GetMagnitude() const
{
	float mag;

	mag = x * x + y * y + z * z;
	mag = sqrtf(mag);

	return mag;
}

inline const Vector3& Vector3::Normalize()
{
	float mag = GetMagnitude();
	float inverseMag = 1.0f / mag;


	//to check if the value inverseMag is not eqaul to infinity 
	if (0.0f * inverseMag == 0.0f * inverseMag)
	{
		x = x * inverseMag;
		y = y * inverseMag;
		z = z * inverseMag;
	}

	return *this;
}



inline bool Vector3::IsValid() const
{
	if (x * 0.0f != x * 0.0f)
		return false;

	if (y * 0.0f != y * 0.0f)
		return false;

	if (z * 0.0f != z * 0.0f)
		return false;

	return true;
}

inline void Vector3::GetOrtho(Vector3& u, Vector3& v) const
{
	Vector3 n = *this;
	n.Normalize();

	const Vector3 w = (n.z * n.z > 0.9f * 0.9f) ? Vector3(1, 0, 0) : Vector3(0, 0, 1);
	u = w.Cross(n);
	u.Normalize();

	v = n.Cross(u);
	v.Normalize();
	u = v.Cross(n);
	u.Normalize();
}







/// <summary>
///  Vector 4 Maths Libary 
/// </summary>
class Vector4
{
public:
	Vector4();

	float x;
	float y;
	float z;
	float w;

	Vector4(const float value);
	Vector4(const Vector4& rhs);
	Vector4(float X, float Y, float Z, float W);
	Vector4(const float* rhs);

	Vector4& operator = (const Vector4& rhs);

	bool operator == (const Vector4& rhs) const;
	bool operator != (const Vector4& rhs) const;

	Vector4	operator + (const Vector4& rhs) const;
	Vector4	operator - (const Vector4& rhs) const;
	Vector4	operator * (const float rhs) const;
	const Vector4& operator += (const Vector4& rhs);
	const Vector4& operator -= (const Vector4& rhs);
	const Vector4& operator *= (const Vector4& rhs);
	const Vector4& operator /= (const Vector4& rhs);

	float operator [] (const int idx) const;
	float& operator [] (const int idx);

	float Dot(const Vector4& rhs) const;
	float GetMagnitude() const;
	const Vector4& Normalize();
	bool IsValid() const;
	void Zero() { x = 0; y = 0; z = 0; w = 0; }

	const float* ToPtr() const { return &x; }
	float* ToPtr() { return &x; }
};

inline Vector4::Vector4() : x(0), y(0), z(0), w(0) {

}

inline Vector4::Vector4(const float value) : x(value), y(value), z(value), w(value) {

}

inline Vector4::Vector4(const Vector4& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {

}

inline Vector4::Vector4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {

}

inline Vector4::Vector4(const float* rhs)
{
	x = rhs[0];
	y = rhs[1];
	z = rhs[2];
	w = rhs[3];
}


inline Vector4& Vector4::operator=(const Vector4& rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	w = rhs.w;
	return *this;
}

inline bool Vector4::operator ==(const Vector4& rhs) const
{
	if (x != rhs.x)
		return false;

	if (y != rhs.y)
		return false;

	if (z != rhs.z)
		return false;

	if (w != rhs.w)
		return false;

	return true;
}


inline bool Vector4::operator !=(const Vector4& rhs) const
{
	if (*this == rhs)
		return false;


	return true;
}

inline Vector4 Vector4::operator +(const Vector4& rhs) const
{
	Vector4 tempVector;

	tempVector.x = x + rhs.x;
	tempVector.y = y + rhs.y;
	tempVector.z = z + rhs.z;
	tempVector.w = w + rhs.w;

	return tempVector;
}

inline Vector4 Vector4::operator -(const Vector4& rhs) const
{
	Vector4 tempVector;

	tempVector.x = x - rhs.x;
	tempVector.y = y - rhs.y;
	tempVector.z = z - rhs.z;
	tempVector.w = w - rhs.w;

	return tempVector;
}

inline Vector4 Vector4::operator *(const float rhs) const
{
	Vector4 tempVector;

	tempVector.x = x * rhs;
	tempVector.y = y * rhs;
	tempVector.z = z * rhs;
	tempVector.w = w * rhs;

	return tempVector;
}

inline const Vector4& Vector4::operator +=(const Vector4& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	w += rhs.w;
	return *this;
}

inline const Vector4& Vector4::operator -=(const Vector4& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	w -= rhs.w;
	return *this;
}

inline const Vector4& Vector4::operator *=(const Vector4& rhs)
{
	x *= rhs.x;
	y *= rhs.y;
	z *= rhs.z;
	w *= rhs.w;
	return *this;
}

inline const Vector4& Vector4::operator /=(const Vector4& rhs)
{
	x /= rhs.x;
	y /= rhs.y;
	z /= rhs.z;
	w /= rhs.w;
	return *this;
}

inline float Vector4::operator[] (const int idx) const
{
	assert(idx >= 0 && idx < 4);
	return (&x)[idx];
}


inline float& Vector4::operator[] (const int idx)
{
	assert(idx >= 0 && idx < 4);
	return (&x)[idx];
}


inline float Vector4::Dot(const Vector4& rhs) const
{
	float xx = x * rhs.x;
	float yy = y * rhs.y;
	float zz = z * rhs.z;
	float ww = w * rhs.w;
	return (xx + yy + zz + ww);
}


inline float Vector4::GetMagnitude() const
{
	float mag;

	mag = x * x + y * y + z * z + w * w;
	mag = sqrtf(mag);
}

inline const Vector4& Vector4::Normalize()
{
	float mag = GetMagnitude();
	float inverseMag = 1.0f / mag;

	if (0.0f * inverseMag == 0.0f * inverseMag)
	{
		x *= inverseMag;
		y *= inverseMag;
		z *= inverseMag;
		w *= inverseMag;
	}

	return *this;
}

inline bool Vector4::IsValid() const
{
	if (x * 0.0f != x * 0.0f)
		return false;

	if (y * 0.0f != y * 0.0f)
		return false;

	if (z * 0.0f != z * 0.0f)
		return false;

	if (w * 0.0f != w * 0.0f)
		return false;

	return true;
}