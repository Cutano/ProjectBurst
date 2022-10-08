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
        
        m_ThreadPool->submit([this, path]
        {
            m_Image->SaveAsPNG(path);
        });
    }

    void Renderer::RenderJob(const RenderJobContext context)
    {
        const int rowBegin = context.rowBeginIndex;
        const int rowEnd = context.rowPostEndIndex;
        for (int row = rowBegin; row < rowEnd; ++row)
        {
            for (int col = 0; col < context.image->GetWidth(); ++col)
            {
                context.image->At(row, col) = {static_cast<unsigned char>(row / 4), static_cast<unsigned char>(0x00), static_cast<unsigned char>(0x00)};
            }

            ++s_DoneCnt;
            context.callback(static_cast<float>(s_DoneCnt) / static_cast<float>(context.image->GetHeight()), context.image->GetRawData());
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

} // namespace Burst
