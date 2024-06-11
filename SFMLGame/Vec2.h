#pragma once
#include <cmath>
#include <ostream>

class Vec2 {
public:
	float x = 0;
	float y = 0;
	Vec2();
	Vec2(float xin, float yin);

	bool operator ==	(const Vec2& rhs) const;
	bool operator !=	(const Vec2& rhs) const;

	Vec2 operator +		(const Vec2& rhs) const;
	Vec2 operator -		(const Vec2& rhs) const;
	Vec2 operator /		(const float val) const;
	Vec2 operator *		(const float val) const;

	void operator +=	(const Vec2& rhs);
	void operator -=	(const Vec2& rhs);
	void operator *=	(const float val);
	void operator /=	(const float val);

	friend std::ostream& operator<<(std::ostream& out, Vec2 const& m);

	Vec2 difference(const Vec2& vec) const;
	float dist(const Vec2& vec) const;
	float angle(const Vec2& vec) const;
	Vec2 normalize(const Vec2& target) const;
	Vec2 getRelPos(const float dist, const float dir) const;
	float length() const;
};