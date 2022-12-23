#ifndef MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
#define MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA

#include <cmath>
#include <cassert>
#include <cstdlib>

#include "vec3.hpp"
#include "vec4.hpp"

#define PI acos(-1)
/** Mat44f: 4x4 matrix with floats
 *
 * See vec2f.hpp for discussion. Similar to the implementation, the Mat44f is
 * intentionally kept simple and somewhat bare bones.
 *
 * The matrix is stored in row-major order (careful when passing it to OpenGL).
 *
 * The overloaded operator () allows access to individual elements. Example:
 *    Mat44f m = ...;
 *    float m12 = m(1,2);
 *    m(0,3) = 3.f;
 *
 * The matrix is arranged as:
 *
 *   ⎛ 0,0  0,1  0,2  0,3 ⎞
 *   ⎜ 1,0  1,1  1,2  1,3 ⎟
 *   ⎜ 2,0  2,1  2,2  2,3 ⎟
 *   ⎝ 3,0  3,1  3,2  3,3 ⎠
 */
struct Mat44f
{
	float v[16];

	constexpr
	float& operator() (std::size_t aI, std::size_t aJ) noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
	constexpr
	float const& operator() (std::size_t aI, std::size_t aJ) const noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
};

// Identity matrix
constexpr Mat44f kIdentity44f = { {
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
} };

constexpr Mat44f zero44f = { {
	0.f, 0.f, 0.f, 0.f,
	0.f, 0.f, 0.f, 0.f,
	0.f, 0.f, 0.f, 0.f,
	0.f, 0.f, 0.f, 0.f
} };

constexpr Mat44f one44f = { {
	1.f, 1.f, 1.f, 1.f,
	1.f, 1.f, 1.f, 1.f,
	1.f, 1.f, 1.f, 1.f,
	1.f, 1.f, 1.f, 1.f
} };

// Common operators for Mat44f.
// Note that you will need to implement these yourself.

constexpr
Mat44f operator*( Mat44f const& aLeft, Mat44f const& aRight ) noexcept
{
	Mat44f result = zero44f;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int a = 0; a < 4; a++) {
				result(i, j) += aLeft(i,a) * aRight(a,j);
			}
		}
	}
	return result;
}

constexpr
Vec4f operator*( Mat44f const& aLeft, Vec4f const& aRight ) noexcept
{
	Vec4f result{0,0,0,0};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result[i] += aLeft(i, j) * aRight[j];
		}
	}
	return result;
}

// Functions:

inline
Mat44f make_rotation_x( float aAngle ) noexcept
{
	Mat44f result = kIdentity44f;
	float radians = aAngle*PI/180;
	result(1, 1) = cos(radians);
	result(1, 2) = -sin(radians);
	result(2, 1) = sin(radians);
	result(2, 2) = cos(radians);
	return result;
}


inline
Mat44f make_rotation_y( float aAngle ) noexcept
{
	Mat44f result = kIdentity44f;
	float radians = aAngle * PI / 180;
	result(0, 0) = cos(radians);
	result(0, 2) = sin(radians);
	result(2, 0) = -sin(radians);
	result(2, 2) = cos(radians);
	return result;
}

inline
Mat44f make_rotation_z( float aAngle ) noexcept
{
	Mat44f result = kIdentity44f;
	float radians = aAngle * PI / 180;
	result(0, 0) = cos(radians);
	result(0, 1) = -sin(radians);
	result(1, 0) = sin(radians);
	result(1, 1) = cos(radians);
	return result;
}

inline
Mat44f make_translation( Vec3f aTranslation ) noexcept
{
	Mat44f result = kIdentity44f;
	result(0, 3) = aTranslation.x;
	result(1, 3) = aTranslation.y;
	result(2, 3) = aTranslation.z;
	return result;
}

inline
Mat44f make_scaling( float aSX, float aSY, float aSZ ) noexcept
{
	Mat44f result = kIdentity44f;
	result(0, 0) = aSX;
	result(1, 1) = aSY;
	result(2, 2) = aSZ;
	return result;
}


inline
Mat44f make_perspective_projection( float aFovInRadians, float aAspect, float aNear, float aFar ) noexcept
{
	assert(aAspect != 0.0);
	assert(aFar != aNear);
	//float tanHalfFovy = tan(aFovInRadians / 2.0);
	//Mat44f result = zero44f;
	//result(0,0) = 1.0 / (aAspect * tanHalfFovy);
	//result(1,1) = 1.0 / (tanHalfFovy);
	//result(2,2) = -(aFar + aNear) / (aFar - aNear);
	//result(2,3) = -1.0;
	//result(3,2) = -(2.0 * aFar * aNear) / (aFar - aNear);
	//return result;



	float cotHalfFovy = 1.0/ tan(aFovInRadians / 2.0);
	Mat44f result = zero44f;
	result(0, 0) = cotHalfFovy / aAspect;
	result(1, 1) = cotHalfFovy;
	result(2, 2) = (aNear + aFar) / (aNear - aFar);
	result(3,2) = -1.0;
	result(2,3) =-(2.0 * aFar * aNear) / (aFar - aNear);
	return result;
}



#endif // MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA

