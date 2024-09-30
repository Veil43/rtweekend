#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

// Expects rtweekend.h to work

#include "hittable.hpp"

#include <vector>

class HittableList : public Hittable {
public:
	std::vector<shared_ptr<Hittable>> m_Objects;

	HittableList() {}
	HittableList(shared_ptr<Hittable> object) { add(object); }

	void clear() { m_Objects.clear(); }

	void add(shared_ptr<Hittable> object) {
		m_Objects.push_back(object);
	}

	bool hit(const Ray& r, Interval ray_param_range, HitRecord& rec) const override {
		HitRecord temp_rec;
		bool hit_anything = false;
		auto closest_so_far = ray_param_range.m_Max;

		for (const auto& object : m_Objects) {
			if (object->hit(r, Interval{ ray_param_range.m_Min, closest_so_far }, temp_rec)) {
				hit_anything = true;
				closest_so_far = temp_rec.m_Param;
				rec = temp_rec;
			}
		}

		return hit_anything;
	}

};
#endif // !HITTABLE_LIST_H
