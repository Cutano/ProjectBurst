#include "Renderer.h"
#include "Scene/Scene.h"
#include "Image.h"

namespace Burst
{
    Renderer::Renderer()
    {

    }

    Renderer::~Renderer()
    {

    }

    void Renderer::Render(int width, int height, Scene &scene, std::function<void(float, Pixel*)> callback)
    {
        Image img(width, height);

        callback(0, img.GetRawData());
    }

} // namespace Burst
