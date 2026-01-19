#pragma once

#include <ostream>
class Vector
{
public:
	Vector();
	Vector(int x, int y);
	Vector(const Vector& v);
	virtual ~Vector() {}

	// basic functions
	float	dist(const Vector& v) const;
	bool	empty() { return (x == 0 && y == 0) ? true : false; }

	// operators
	Vector& operator=(const Vector& v);
	Vector operator+(const Vector& v);
	Vector operator-(const Vector& v);
	Vector operator-();
	Vector& operator+=(const Vector& v);
	Vector& operator-=(const Vector& v);
	bool operator==(const Vector& v) const;

	friend std::ostream& operator<<(std::ostream& out, const Vector& v);

	// data - public
	int     x{ 0 }, y{ 0 };
};

// additional operators for arithmetic and stream operations
Vector operator+(const Vector& lv, const Vector& rv);
Vector operator-(const Vector& lv, const Vector& rv);

