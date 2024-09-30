#ifndef RAY_H
#define RAY_H

#include "vec3.hpp"

class Ray {
public:
	Ray() {}
	Ray(const Point3& origin, const Vec3& direction) : m_Origin{ origin }, m_Direction{ direction } {}

	const Point3& origin() const { return m_Origin; }
	const Vec3& direction() const { return m_Direction; }

	Point3 at(double scalar) const {
	/*
	* Returns a point on the line of the ray when lambda == scalar
	*/
		return m_Origin + scalar * m_Direction;
	}

private:
	Point3 m_Origin;
	Vec3 m_Direction;
};

#endif