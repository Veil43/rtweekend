#ifndef SPHERE_H
#define SPHERE_H

// Expects rtweekend.h to work

#include "hittable.hpp"

class Sphere : public Hittable {
public:
	Sphere(const Point3& center, double radius, shared_ptr<Material> mat) 
		: m_Center{ center }, m_Radius{ std::fmax(0, radius) }, m_Mat{ mat } {}

	bool hit(const Ray& r, Interval ray_param_range, HitRecord& rec) const override {
		// NOTE: a h c -> quadratic formula component where b = -2h
		Vec3 oc = m_Center - r.origin();
		auto a = r.direction().lengthSquared();
		auto h = dot(r.direction(), oc);
		auto c = oc.lengthSquared() - m_Radius * m_Radius;
  
		auto discriminant = h * h - a * c;
		if (discriminant < 0)
			return false;

		auto sqrtd = std::sqrt(discriminant);

		// Find the nearest root that lies in the acceptable range.
		// This root is the scalar paramater for ray vector
		auto root = (h - sqrtd) / a;
		if (not ray_param_range.surrounds(root)) {
			root = (h + sqrtd) / a;
			if (not ray_param_range.surrounds(root))
				return false;
		}

		// Record which side was hit and set the normal
		rec.m_Param = root;
		rec.m_Hit_point = r.at(rec.m_Param);
		Vec3 outward_normal = (rec.m_Hit_point - m_Center) / m_Radius;
		rec.setFaceNormal(r, outward_normal);
		rec.m_Mat = m_Mat;

		return true;
	}
private:
	Point3 m_Center;
	double m_Radius;
	shared_ptr<Material> m_Mat;
};
#endif // !SPHERE_H
