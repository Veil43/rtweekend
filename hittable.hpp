#ifndef HITTABLE_H
#define HITTABLE_H

// Expects rtweekend.h to work

class Material;

class HitRecord {
public:
	Point3 m_Hit_point;
	Vec3 m_Normal;
	shared_ptr<Material> m_Mat;
	double m_Param;
	bool m_Front_face;

	void setFaceNormal(const Ray& r, const Vec3& outward_normal) {
		// Set the hit record normal vector.
		// NOTE: the parameter 'outward_normal' is assumed to have unit length.

		m_Front_face = dot(r.direction(), outward_normal) < 0; // Look at projection of vectors
		m_Normal = m_Front_face ? outward_normal : -outward_normal;
	}
};

class Hittable {
public:
	virtual ~Hittable() = default;
	virtual bool hit(const Ray& r, Interval ray_param_range, HitRecord& rec) const = 0;
};

#endif // !HITTABLE_H
