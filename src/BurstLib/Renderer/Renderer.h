#pragma once

#include <functional>
#include <memory>
#include <string>
#include <atomic>

#include "Common.h"

namespace BS
{
    template <typename T> class multi_future;
    class thread_pool;
}

namespace Burst
{
    class Scene;
    class Pixel;
    class Image;

    struct RenderJobContext
    {
        int rowBeginIndex;
        int rowPostEndIndex;
        std::shared_ptr<Image> image;
        std::shared_ptr<Scene> scene;
        std::function<void(float, Pixel*)> callback;
    };

    class Renderer
    {
    private:
        std::unique_ptr<BS::thread_pool> m_ThreadPool;
        std::shared_ptr<Image> m_Image;

        inline static std::atomic<int> s_DoneCnt {0};
        static void RenderJob(RenderJobContext context);
        
    public:
        Renderer();
        ~Renderer();

        BS::multi_future<void> Render(int width, int height, const std::shared_ptr<Scene>& scene, const std::function<void(float, Pixel*)>& callback);
        void SaveImage(const std::string& path) const;
    };
} // namespace Burst
