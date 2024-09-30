#ifndef MATERIAL_H
#define MATERIAL_H

class Material {
public:
	virtual ~Material() = default;

	virtual bool scatter(
		const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered
	) const {
		return false;
	}
};

class Lambertian : public Material {
public:
	Lambertian(const Color& albedo) : m_Albedo{ albedo } {}

	bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered)
		const override {
		auto scatter_direction = rec.m_Normal + randomUnitVector();

		// Catch degenerate scatter direction (random vect is close to -normal)
		if (scatter_direction.nearZero())
			scatter_direction = rec.m_Normal;

		scattered = Ray(rec.m_Hit_point, scatter_direction);
		attenuation = m_Albedo;
		return true;
	}
private:
	Color m_Albedo;
};

class Metal : public Material {
public:
	Metal(const Color& albedo, double fuzz) : m_Albedo{ albedo }, m_Fuzz{ fuzz } {}

	bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered)
		const override {
		auto reflected_direction = reflect(r_in.direction(), rec.m_Normal);
		reflected_direction = unitVector(reflected_direction) + (m_Fuzz * randomUnitVector());

		scattered = Ray(rec.m_Hit_point, reflected_direction);
		attenuation = m_Albedo;
		return (dot(scattered.direction(), rec.m_Normal) > 0); // absorb fuzzy rays that point inwards (DP < 0)
	}
private:
	Color m_Albedo;
	double m_Fuzz;
};

class Dielectric : public Material {
public:
	Dielectric(double refraction_index) : m_Refraction_index{ refraction_index } {}

	bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered)
		// TODO: understand Snell's law and refraction.
		// Go back and understand how this dielectric material is working
		const override {
		attenuation = Color{ 1.0, 1.0, 1.0 };
		double ri = rec.m_Front_face ? (1.0 / m_Refraction_index) : m_Refraction_index;

		Vec3 unit_direction = unitVector(r_in.direction());
		double cos_theta = std::fmin(dot(-unit_direction, rec.m_Normal), 1.0);
		double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

		bool cannot_refract = ri * sin_theta > 1.0;
		Vec3 direction;

		if (cannot_refract or reflectance(cos_theta, ri) > randomDouble())
			direction = reflect(unit_direction, rec.m_Normal);
		else 
			direction = refract(unit_direction, rec.m_Normal, ri);

		scattered = Ray(rec.m_Hit_point, direction);
		return true;
	}
private:
	// Refractive index in vacuum or air, or the ratio of the material's refractive index over
	// the refractive index of the enclosing media.
	double m_Refraction_index;

	static double reflectance(double cosine, double refraction_index) {
		// Use Schlick's approximation for reflectance.
		auto r0 = (1 - refraction_index) / (1 + refraction_index);
		r0 = r0 * r0;
		return r0 + (1 - r0) * std::pow((1 - cosine), 5);
	}
};

#endif // !MATERIAL_H
