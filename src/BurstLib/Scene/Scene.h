#pragma once

#include <memory>

namespace Burst
{
    class camera;
    
    class Scene
    {
    public:
        Scene();
        ~Scene();

    private:
        std::shared_ptr<camera> m_Camera;
    };
}
