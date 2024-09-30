// WeekendRT.cpp : Defines the entry point for the application.
//

#include "rtweekend.h"

#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "sphere.hpp"

int main() {
	HittableList world;

	/*auto material_ground = make_shared<Lambertian>(Color{ 0.8, 0.8, 0.0 });
	auto material_center = make_shared<Lambertian>(Color{ 0.1, 0.2, 0.5 });
	auto material_left   = make_shared<Dielectric>(1.50);
	auto material_bubble = make_shared<Dielectric>(1.00 / 1.50);
	auto material_right  = make_shared<Metal>(Color{ 0.8, 0.6, 0.2 }, 1.0);

	world.add(make_shared<Sphere>(Point3{  0.0, -100.5, -1.0 }, 100.0, material_ground));
	world.add(make_shared<Sphere>(Point3{  0.0,	   0.0, -1.2 },   0.5, material_center));
	world.add(make_shared<Sphere>(Point3{ -1.0,    0.0, -1.0 },   0.5, material_left));
	world.add(make_shared<Sphere>(Point3{ -1.0,    0.0, -1.0 },   0.4, material_bubble));
	world.add(make_shared<Sphere>(Point3{  1.0,    0.0, -1.0 },   0.5, material_right));*/

	auto material_ground = make_shared<Lambertian>(Color{ 0.5, 0.5, 0.5 });
	world.add(make_shared<Sphere>(Point3{ 0, -1000, 0 }, 1000, material_ground));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = randomDouble();
			Point3 center(a + 0.9 * randomDouble(), 0.2, b + 0.9 * randomDouble());

			if ((center - Point3{ 4, 0.2, 0 }).length() > 0.9) {
				shared_ptr<Material> material_sphere;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = Color::random() * Color::random();
					material_sphere = make_shared<Lambertian>(albedo);
					world.add(make_shared<Sphere>(center, 0.2, material_sphere));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = Color::random(0.5, 1);
					auto fuzz = randomDouble(0, 0.5);
					material_sphere = make_shared<Metal>(albedo, fuzz);
					world.add(make_shared<Sphere>(center, 0.2, material_sphere));
				}
				else {
					// glass
					material_sphere = make_shared<Dielectric>(1.5);
					world.add(make_shared<Sphere>(center, 0.2, material_sphere));
				}
			}
		}

		auto material1 = make_shared<Dielectric>(1.5);
		world.add(make_shared<Sphere>(Point3{ 0, 1, 0 }, 1.0, material1));

		auto material2 = make_shared<Lambertian>(Color{ 0.4, 0.2, 0.1 });
		world.add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

		auto material3 = make_shared<Metal>(Color{ 0.7, 0.6, 0.5 }, 0.0);
		world.add(make_shared<Sphere>(Point3{ 4, 1, 0 }, 1.0, material3));
	}

	Camera cam;

	cam.m_Aspect_ratio		= 16.0 / 9.0;
	cam.m_Image_width		= 1200;
	cam.m_Samples_per_pixel = 500;
	cam.m_Max_depth			= 50;

	// how does fov change viewport vector computations?
	cam.m_Vfov		= 20;
	cam.m_Look_From = Point3{ 12, 2, 3 };
	cam.m_Look_at	= Point3{ 0, 0, 0 };
	cam.m_Vup		= Vec3{ 0, 1, 0 };

	//TODO: understand how the focus and defocus work
	cam.m_Defocus_angle = 0.6;
	cam.m_Focus_dist	= 10.0;

	cam.render(world);

	return 0;
}