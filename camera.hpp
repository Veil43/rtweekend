#ifndef CAMERA_H
#define CAMERA_H

// Expects rtweekend.h to work

#include "hittable.hpp"
#include "material.hpp"

class Camera {
public:
	// Image
	double	m_Aspect_ratio		= 1.0;	// Ratio of image width over height
	int		m_Image_width		= 100;	// Rendered image width in pixel count
	int		m_Samples_per_pixel = 10;	// Count of random samples for each pixel
	int		m_Max_depth			= 10;   // Maximum number of ray bounces into a scene
	
	double m_Vfov		= 90;					// Vertical view angle (field of view)
	Point3 m_Look_From  = Point3{ 0, 0, 0 };	// Point camera is looking from
	Point3 m_Look_at	= Point3{ 0, 0, -1 };	// Point camera is looking at
	Vec3   m_Vup		= Point3{ 0, 1, 0 };	// Camera-relative "up" direction

	// TODO: understand how focus and defocus work
	double m_Defocus_angle	= 0;		// Variation angle of rays through each pixel
	double m_Focus_dist		= 10;		// Distance from camera lookfrom point to plane of perfect focus

	void render(const Hittable& world) {
		initialize();
		
		std::cout << "P3\n" << m_Image_width << ' ' << m_Image_height << "\n255\n";

		for (int j = 0; j < m_Image_height; j++) {
			std::clog << "\rScanlines remaining: " << (m_Image_height - j) << '/' << m_Image_height << "  " << std::flush; // Progress
			for (int i = 0; i < m_Image_width; i++) {
				Color pixel_color{ 0, 0, 0 };
				for (int sample = 0; sample < m_Samples_per_pixel; sample++) {
					Ray r = getRay(i, j);
					pixel_color += rayColor(r, m_Max_depth, world);
				}
				writeColor(std::cout, m_Pixel_samples_scale * pixel_color);
			}
		}
		std::clog << "\rDone!                      \n";
	}

private:
	int		m_Image_height;			// Rendered image height
	double	m_Pixel_samples_scale;	// Color scale factor for a sum of pixel samples (for the average)
	Point3	m_Camera_center;		// Camera center
	Point3	m_Pixel00_loc;			// Location of pixel 0 0
	Vec3	m_Pixel_delta_u;		// Offset to pixel to the right
	Vec3	m_Pixel_delta_v;		// Offset to pixel below
	Vec3	m_U, m_V, m_W;			// Camera frame basis  vectors
	Vec3	m_Defocus_disk_u;		// Defocus disk horizontal radius
	Vec3	m_Defocus_disk_v;		// Defocus disk vertical radius

	void initialize() {
		// Calculate the image height, and ensure that it's at least 1.
		m_Image_height = int(m_Image_width / m_Aspect_ratio);
		m_Image_height = (m_Image_height < 1) ? 1 : m_Image_height;

		m_Pixel_samples_scale = 1.0 / m_Samples_per_pixel;

		m_Camera_center = m_Look_From;

		// Determine viewport dimensions.
		// auto focal_length = (m_Look_From - m_Look_at).length(); // Viewport to camera distance (before defocus)
		auto theta = degreesToRadians(m_Vfov);
		auto h = std::tan(theta / 2);
		auto viewport_height = 2.0 * h * m_Focus_dist;
		auto viewport_width = viewport_height * (double(m_Image_width) / m_Image_height);

		// Calculate the u, v, w unit basis vectors for the camera coordinate frame.
		m_W = unitVector(m_Look_From - m_Look_at);
		m_U = unitVector(cross(m_Vup, m_W));
		m_V = cross(m_W, m_U);

		// Calculate the vectors across the horizontal and down the vertical viewport edges.
		auto viewport_u = viewport_width * m_U;		// Vector acros viewport horizontal edge;
		auto viewpport_v = viewport_height * -m_V;	// Vector acros viewport vertical edge

		// Calculate the horizontal and vertical delta vectors from pixel to pixel.
		m_Pixel_delta_u = viewport_u / m_Image_width;
		m_Pixel_delta_v = viewpport_v / m_Image_height;

		// Find the location of the upper left pixel (0,0 on the viewport)
		// Reminder: the viewport is in the negaitve z direction
		auto viewport_upper_left = m_Camera_center - (m_Focus_dist * m_W) - viewport_u / 2 - viewpport_v / 2;
		m_Pixel00_loc = viewport_upper_left + 0.5 * (m_Pixel_delta_u + m_Pixel_delta_v);

		// TODO: understand defocus
		// Calculate the camera defocus disk basis vectors.
		auto defocus_radius = m_Focus_dist * std::tan(degreesToRadians(m_Defocus_angle / 2));
		m_Defocus_disk_u = m_U * defocus_radius;
		m_Defocus_disk_v = m_V * defocus_radius;
	}

	Ray getRay(int i, int j) const {
		// Construct a camera ray originating from the defocus disk and directed at a
		// randomly sampled points around the pixel location i, j. (instead of dead center)

		auto offset = sampleSquare();
		auto pixel_sample = m_Pixel00_loc
			+ ((i + offset.x()) * m_Pixel_delta_u)
			+ ((j + offset.y()) * m_Pixel_delta_v);

		auto ray_origin = (m_Defocus_angle <= 0 )? m_Camera_center : defocusDiskSample();
		auto ray_direction = pixel_sample - ray_origin;
		return Ray(ray_origin, ray_direction);
	}

	Vec3 sampleSquare() const {
		// Returns the vector to a random point in the [-.5, -.5] -- [+.5, +.5] unit square.
		return Vec3{ randomDouble() - 0.5, randomDouble() - 0.5, 0 };
	}

	Point3 defocusDiskSample() const {
		auto p = randomVecInUnitDisk();
		return m_Camera_center + (p[0] * m_Defocus_disk_u) + (p[1] * m_Defocus_disk_v);
	}

	Color rayColor(const Ray& r, int depth, const Hittable& world) {
		// If we've exceeded the ray bounce limit, no more light is gathered.
		if (depth <= 0) return Color{ 0, 0, 0 };

		HitRecord rec;

		if (world.hit(r, Interval{ 0.001, infinity }, rec)) {
			Ray scattered;
			Color attenuation; // albedo of the material [0,1) determines how much each color is reflected

			if (rec.m_Mat->scatter(r, rec, attenuation, scattered))
				return attenuation * rayColor(scattered, depth - 1, world);
			return Color{ 0, 0, 0 };
		}

		// Look up how Lerp (linear interpolation) works. particulary the role of the "a"
		Vec3 unit_direction = unitVector(r.direction());
		auto a = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - a) * Color { 1.0, 1.0, 1.0 } + a * Color{ 0.5, 0.7, 1 };
	}
};

#endif // !CAMERA_H
