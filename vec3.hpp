#ifndef VEC3_H
#define VEC3_H

class Vec3 {
public:
	// Disclaimer: Don't ask why any of this is public
	double m_Elements[3];

	Vec3() : m_Elements{ 0, 0, 0 } {}
	Vec3(double e0, double e1, double e2) : m_Elements{ e0, e1, e2 } {}

	double x() const { return m_Elements[0]; }
	double y() const { return m_Elements[1]; }
	double z() const { return m_Elements[2]; }

	Vec3 operator-() const { return Vec3{ -m_Elements[0], -m_Elements[1], -m_Elements[2] }; }
	double operator[](int i) const { return m_Elements[i]; }
	double& operator[](int i) { return m_Elements[i]; }

	Vec3& operator+=(const Vec3& v) {
		m_Elements[0] += v.m_Elements[0];
		m_Elements[1] += v.m_Elements[1];
		m_Elements[2] += v.m_Elements[2];
		return *this;
	}

	Vec3& operator*=(double scalar) {
		m_Elements[0] *= scalar;
		m_Elements[1] *= scalar;
		m_Elements[2] *= scalar;
		return *this;
	}

	Vec3& operator/=(double scalar) {
		return *this *= 1 / scalar;
	}

	double length() const {
		return std::sqrt(lengthSquared());
	}

	double lengthSquared() const {
		return m_Elements[0] * m_Elements[0] + m_Elements[1] * m_Elements[1] + m_Elements[2] * m_Elements[2];
	}

	bool nearZero() const {
		// Return true if the vector is clos to zero in all dimensions
		auto s = 1e-8;
		return (std::fabs(m_Elements[0]) < s) and (std::fabs(m_Elements[1]) < s) and (std::fabs(m_Elements[2]) < s);
	}

	static Vec3 random() {
		return Vec3{ randomDouble(), randomDouble(), randomDouble() };
	}

	static Vec3 random(double min, double max) {
		return Vec3{ randomDouble(min, max), randomDouble(min, max), randomDouble(min, max) };
	}
};

// Point3 is just an alias for Vec3, but useful for geometric clarity in the code.
using Point3 = Vec3;

// Vector Utility Functions

inline std::ostream& operator<<(std::ostream& out, const Vec3& v) {
	return out << v.m_Elements[0] << ' ' << v.m_Elements[1] << ' ' << v.m_Elements[2];
}

inline Vec3 operator+(const Vec3& u, const Vec3& v) {
	return Vec3{ u.m_Elements[0] + v.m_Elements[0], u.m_Elements[1] + v.m_Elements[1], u.m_Elements[2] + v.m_Elements[2] };
}

inline Vec3 operator-(const Vec3& u, const Vec3& v) {
	return Vec3{ u.m_Elements[0] - v.m_Elements[0], u.m_Elements[1] - v.m_Elements[1], u.m_Elements[2] - v.m_Elements[2] };
}

// Unfamiliar operation! WTF is this???
inline Vec3 operator*(const Vec3& u, const Vec3& v) {
	return Vec3{ u.m_Elements[0] * v.m_Elements[0], u.m_Elements[1] * v.m_Elements[1], u.m_Elements[2] * v.m_Elements[2] };
}

inline Vec3 operator*(double scalar, const Vec3& v) {
	return Vec3{ scalar * v.m_Elements[0], scalar * v.m_Elements[1], scalar * v.m_Elements[2] };
}

inline Vec3 operator*(const Vec3& v, double scalar) {
	return scalar * v;
}

inline Vec3 operator/(const Vec3& v, double scalar) {
	return (1 / scalar) * v;
}

inline double dot(const Vec3& u, const Vec3& v) {
	return u.m_Elements[0] * v.m_Elements[0]
		+  u.m_Elements[1] * v.m_Elements[1]
		+  u.m_Elements[2] * v.m_Elements[2];
}

inline Vec3 cross(const Vec3& u, const Vec3& v) {
	return Vec3{ u.m_Elements[1] * v.m_Elements[2] - u.m_Elements[2] * v.m_Elements[1],
				 u.m_Elements[2] * v.m_Elements[0] - u.m_Elements[0] * v.m_Elements[2],
				 u.m_Elements[0] * v.m_Elements[1] - u.m_Elements[1] * v.m_Elements[0] };
}

inline Vec3 unitVector(const Vec3& v) {
	return v / v.length();
}


inline Vec3 randomUnitVector() {
	// TODO: Implement a way to find vectors withing the sphere not the square.
	while (true) {
		auto p = Vec3::random(-1, 1);
		auto lensq = p.lengthSquared();
		if (1e-160 < lensq and lensq <= 1) return p / sqrt(lensq);

		// Vectors really close to the center may yield 0 length
		// so we block out that zone. With 64 bits (doubles) we can support from 10^-160 and up.
	}
}

inline Vec3 randomVecOnHemisphere(const Vec3& normal) {
	Vec3 on_unit_sphere = randomUnitVector();
	if (dot(on_unit_sphere, normal) > 0.0) // Same hemisphere as normal
		return on_unit_sphere;
	else
		return -on_unit_sphere;
}

inline Vec3 randomVecInUnitDisk() {
	while (true) {
		auto p = Vec3{ randomDouble(-1, 1), randomDouble(-1, 1), 0 };
		if (p.lengthSquared() < 1) return p;
	}
}

inline Vec3 reflect(const Vec3& v, const Vec3& n) {
	// The reflection on a shiny surface is supposedly v + 2b
	// where b = proj_n v so b = (v.n) x n.
	// Since v is always comming from outside the b points into the surface so, 
	// reflection direction = v - 2b
	return v - 2 * dot(v, n) * n;
}

inline Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat) {
	// TODO: I don't really understand how this works. look into: Snell's Law and refraction
	// treat the code bellow as a blackbox for now
	auto cos_theta = std::fmin(dot(-uv, n), 1.0);
	Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	Vec3 r_out_parallel = -std::sqrt(std::abs(1.0 - r_out_perp.lengthSquared())) * n;
	return r_out_perp + r_out_parallel;
}
#endif