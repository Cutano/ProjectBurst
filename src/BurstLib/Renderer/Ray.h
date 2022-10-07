#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace Burst
{
    class ray
    {
    public:
        ray() {}
        ray(const glm::vec3 &origin, const glm::vec3 &direction)
            : orig(origin), dir(direction), tm(0)
        {
        }

        ray(const glm::vec3 &origin, const glm::vec3 &direction, double time)
            : orig(origin), dir(direction), tm(time)
        {
        }

        glm::vec3 origin() const { return orig; }
        glm::vec3 direction() const { return dir; }
        double time() const { return tm; }

        glm::vec3 at(double t) const
        {
            return orig + dir * t;
        }

    public:
        glm::vec3 orig;
        glm::vec3 dir;
        double tm;
    };
    
} // namespace Burst
