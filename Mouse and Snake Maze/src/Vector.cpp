#include "Vector.h"
#include <cmath>

Vector::Vector()
	: x(0), y(0)
{
}


//-----------------------------------------------------------------------------
Vector::Vector(int x, int y)
	: x(x), y(y)
{
}

//-----------------------------------------------------------------------------
Vector::Vector(const Vector& v)
	: x(v.x), y(v.y)
{
}

//-----------------------------------------------------------------------------
Vector& Vector::operator=(const Vector& v)
{
	x = v.x;
	y = v.y;
	return *this;
}

//-----------------------------------------------------------------------------
Vector Vector::operator+(const Vector& v)
{
	return Vector(x + v.x, y + v.y);
}

//-----------------------------------------------------------------------------
Vector Vector::operator-(const Vector& v)
{
	return Vector(x - v.x, y - v.y);
}

//-----------------------------------------------------------------------------
Vector Vector::operator-()
{
	return Vector{ -x, -y };
}

//-----------------------------------------------------------------------------

Vector& Vector::operator+=(const Vector& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

//-----------------------------------------------------------------------------
Vector& Vector::operator-=(const Vector& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

//-----------------------------------------------------------------------------
bool   Vector::operator==(const Vector& v) const
{
	if (this->x == v.x && this->y == v.y)
		return true;
	return false;
}

//-----------------------------------------------------------------------------
float Vector::dist(const Vector& v) const
{
	return static_cast<float>(std::sqrt((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y)));
}

//-----------------------------------------------------------------------------
Vector operator+(const Vector& lv, const Vector& rv)
{
	Vector v(lv);
	v += rv;
	return v;
}

//-----------------------------------------------------------------------------
Vector operator-(const Vector& lv, const Vector& rv)
{
	Vector v(lv);
	v -= rv;
	return v;
}

//-----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const Vector& v)
{
	out << "(" << v.x << ", " << v.y << ")";
	return out;
}