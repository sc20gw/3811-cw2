#ifndef MAT22_HPP_1F974C02_D0D1_4FBD_B5EE_A69C88112088
#define MAT22_HPP_1F974C02_D0D1_4FBD_B5EE_A69C88112088

#include <cmath>

#include "vec2.hpp"

/** Mat22f : 2x2 matrix with floats
 *
 * See comments for Vec2f for some discussion.
 *
 * The matrix is stored in row-major order.
 *
 * Example:
 *   Mat22f identity{ 
 *     1.f, 0.f,
 *     0.f, 1.f
 *   };
 */
struct Mat22f
{
	float _00, _01;
	float _10, _11;
};

// Identity matrix
constexpr Mat22f kIdentity22f = {
	1.f, 0.f,
	0.f, 1.f
};

constexpr Mat22f zero22f = {
	0.f, 0.f,
	0.f, 0.f, 
};

constexpr Mat22f one22f = {
	1.f, 1.f,
	1.f, 1.f,
};

// Common operators for Mat22f.
// Note that you will need to implement these yourself.

constexpr
Mat22f operator*( Mat22f const& aLeft, Mat22f const& aRight ) noexcept
{
	Mat22f result = zero22f;
	result._00 = aLeft._00 * aRight._00 + aLeft._01 * aRight._10;
	result._01 = aLeft._00 * aRight._01 + aLeft._01 * aRight._11;
	result._10 = aLeft._10 * aRight._00 + aLeft._11 * aRight._10;
	result._11 = aLeft._10 * aRight._01 + aLeft._11 * aRight._11;
	return result;
}

constexpr
Vec2f operator*( Mat22f const& aLeft, Vec2f const& aRight ) noexcept
{
	Vec2f result{ 0,0 };
	result.x = aLeft._00 * aRight.x + aLeft._01 * aRight.y;
	result.y = aLeft._10 * aRight.x + aLeft._11 * aRight.y;
	return result;
}

// Functions:

inline
Mat22f make_rotation_2d( float aAngle ) noexcept
{
	Mat22f result = zero22f;
	result._00 = cos(aAngle);
	result._01 = sin(aAngle);
	result._10 = -sin(aAngle);
	result._11 = cos(aAngle);
	return result;
}

#endif // MAT22_HPP_1F974C02_D0D1_4FBD_B5EE_A69C88112088
