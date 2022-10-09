#pragma once

#include "../Common.h"
#include "../Hittable.h"

namespace Burst
{
    class sphere : public hittable
    {
    public:
        sphere() {}

        sphere(glm::vec3 cen, double r, shared_ptr<material> m)
            : center(cen), radius(r), mat_ptr(m){}

        bool hit(
            const ray &r, double t_min, double t_max, hit_record &rec) const override;

        bool bounding_box(double time0, double time1, aabb& output_box) const override {return true;}

    public:
        glm::vec3 center;
        double radius;
        shared_ptr<material> mat_ptr;
    };

    bool sphere::hit(const ray &r, double t_min, double t_max, hit_record &rec) const
    {
        glm::vec3 oc = r.origin() - center;
        auto a = length2(r.direction());
        auto half_b = dot(oc, r.direction());
        auto c = length2(oc) - radius * radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0)
            return false;
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (root < t_min || t_max < root)
        {
            root = (-half_b + sqrtd) / a;
            if (root < t_min || t_max < root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        glm::vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;

        return true;
    }
}