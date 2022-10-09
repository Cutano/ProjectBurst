#pragma once

#include "Common.h"

namespace Burst
{
    class camera
    {
    public:
        camera() : camera(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), 40, 1, 0, 10) {}

        camera(
            glm::vec3 lookfrom,
            glm::vec3 lookat,
            glm::vec3 vup,
            double vfov, // vertical field-of-view in degrees
            double aspect_ratio,
            double aperture,
            double focus_dist,
            double _time0 = 0,
            double _time1 = 0)
        {
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta / 2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;

            w = glm::normalize(lookfrom - lookat);
            u = glm::normalize(cross(vup, w));
            v = cross(w, u);

            origin = lookfrom;
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

            lens_radius = aperture / 2;
            time0 = _time0;
            time1 = _time1;
        }

        ray get_ray(double s, double t) const
        {
            glm::vec3 rd = lens_radius * glm::vec3(glm::diskRand(1.0f), 0);
            glm::vec3 offset = u * rd.x + v * rd.y;
            return {
                origin + offset,
                lower_left_corner + s * horizontal + t * vertical - origin - offset,
                random_double(time0, time1)
            };
        }

    private:
        glm::vec3 origin;
        glm::vec3 lower_left_corner;
        glm::vec3 horizontal;
        glm::vec3 vertical;
        glm::vec3 u, v, w;
        double lens_radius;
        double time0, time1; // shutter open/close times
    };
    
} // namespace Burst
