#include "Renderer.h"
#include "Scene/Scene.h"
#include "Image.h"

#include <BS_thread_pool.hpp>

namespace Burst
{
    Renderer::Renderer()
    {
        m_ThreadPool = std::make_unique<BS::thread_pool>();
    }

    Renderer::~Renderer()
    {

    }

    BS::multi_future<void> Renderer::Render(int width, int height, const std::shared_ptr<Scene>& scene, const std::function<void(float, Pixel*)>& callback)
    {
        s_DoneCnt = 0;
        m_Image.reset();
        m_Image = std::make_shared<Image>(width, height);
        BS::multi_future<void> multi_future;

        const int stride = 64;
        const int wholeBlocks = height / stride;
        const int tail = height % stride;
        
        for (int i = 0; i < wholeBlocks; ++i)
        {
            RenderJobContext context {stride * i, stride * (i + 1), m_Image, scene, callback};
            multi_future.push_back(m_ThreadPool->submit(RenderJob, std::move(context)));
        }
        
        RenderJobContext context {wholeBlocks * stride, wholeBlocks * stride + tail, m_Image, scene, callback};
        multi_future.push_back(m_ThreadPool->submit(RenderJob, std::move(context)));
        
        return multi_future;
    }

    void Renderer::SaveImage(const std::string& path) const
    {
        if (m_Image == nullptr)
        {
            return;
        }
        
        auto future = m_ThreadPool->submit([this, path]
        {
            return m_Image->SaveAsPNG(path);
        });
    }

    void Renderer::RenderJob(const RenderJobContext context)
    {
        const int rowBegin = context.rowBeginIndex;
        const int rowEnd = context.rowPostEndIndex;
        
        const auto& scene = context.scene;
        const auto& cam = scene->GetCamera();
        const auto& hitCallback = scene->GetOnHitCallback();
        const auto& world = scene->GetHittableList();
        
        const auto width = context.image->GetWidth();
        const auto height = context.image->GetHeight();

        const int max_depth = 50;
        const int spp = 50;
        
        for (int row = rowBegin; row < rowEnd; ++row)
        {
            for (int col = 0; col < width; ++col)
            {
                glm::vec3 color{0};
                for (int i = 0; i < spp; ++i)
                {
                    auto u = (col + random_double()) / (width - 1);
                    auto v = 1.0 - (row + random_double()) / (height - 1);
                    ray r = cam.get_ray(u, v);
                    color += hitCallback(r, glm::vec3{0}, world, nullptr, max_depth);
                }
                color /= spp;
                const auto& color255 = glm::packUnorm<unsigned char>(color);
                context.image->At(row, col) = {color255.r, color255.g, color255.b};
            }

            ++s_DoneCnt;
            context.callback(static_cast<float>(s_DoneCnt) / static_cast<float>(height), context.image->GetRawData());
        }
    }

} // namespace Burst
