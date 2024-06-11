#include "Vec2.h"

Vec2::Vec2() {}
Vec2::Vec2(float xin, float yin)
	: x{xin}
	, y{yin}
{
}

bool Vec2::operator == (const Vec2& rhs) const {
	return (x == rhs.x && y == rhs.y);
}
bool Vec2::operator != (const Vec2& rhs) const {
	return !(x == rhs.x && y == rhs.y);
}

Vec2 Vec2::operator + (const Vec2& rhs) const {
	return Vec2(x + rhs.x, y + rhs.y);
}
Vec2 Vec2::operator - (const Vec2& rhs) const {
	return Vec2(x - rhs.x, y - rhs.y);
}
Vec2 Vec2::operator / (const float val) const {
	return Vec2(x / val, y / val);
}
Vec2 Vec2::operator * (const float val) const {
	return Vec2(x * val, y * val);
}

void Vec2::operator += (const Vec2& rhs) {
	x += rhs.x;
	y += rhs.y;
}
void Vec2::operator -= (const Vec2& rhs) {
	x -= rhs.x;
	y -= rhs.y;
}
void Vec2::operator *= (const float val) {
	x *= val;
	y *= val;
}
void Vec2::operator /= (const float val) {
	x /= val;
	y /= val;
}

std::ostream& operator << (std::ostream& out, Vec2 const& m) {
	out << "(" << m.x << ", " << m.y << ")";
	return out;
}

Vec2 Vec2::difference(const Vec2& vec) const {
	return Vec2(vec.x - x, vec.y - y);
}
float Vec2::dist(const Vec2& vec) const {
	float a = vec.x - x;
	float b = vec.y - y;
	return std::sqrtf((a * a) + (b * b));
}
float Vec2::angle(const Vec2& vec) const {
	Vec2 diff = this->difference(vec);
	return (atan2f(diff.y, diff.x) * (180.0/3.141592653589793238463));
}
Vec2 Vec2::normalize(const Vec2& vec) const {
	Vec2 diff = this->difference(vec);
	float length = this->dist(vec);
	diff /= length;
	return diff;
}
Vec2 Vec2::getRelPos(const float dist, const float dir) const {
	float radians = dir * (3.141592653589793238463 / 180.0);
	float x = std::cosf(radians) * dist;
	float y = std::sinf(radians) * dist;
	return Vec2(x,y);
}

float Vec2::length() const {
	return std::sqrtf((x * x) + (y * y));
}