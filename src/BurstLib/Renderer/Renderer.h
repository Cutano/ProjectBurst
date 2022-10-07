#pragma once

#include "Common.h"

namespace Burst
{
    class Scene;

    class Renderer
    {
    private:
        /* data */
    public:
        Renderer();
        ~Renderer();

        void Render(int width, int height, Scene& scene);
    };
} // namespace Burst
