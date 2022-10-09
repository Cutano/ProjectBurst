#pragma once

#include <memory>
#include <functional>

#include "../Renderer/Camera.h"
#include "../Renderer/HittableList.h"

namespace Burst
{
    using OnHitFunc = std::function<glm::vec3(const ray&, const glm::vec3&, const hittable&, std::shared_ptr<hittable>, int)>;
    
    class camera;
    
    class Scene
    {
    public:
        Scene();
        explicit Scene(camera camera);
        ~Scene();

        [[nodiscard]] const hittable_list& GetHittableList() const;
        void SetHittableList(hittable_list HittableList);

        [[nodiscard]] camera& GetCamera();
        void SetCamera(const camera& m_camera);
        
        [[nodiscard]] const OnHitFunc& GetOnHitCallback() const;
        void SetOnHitCallback(OnHitFunc onHitCallback);
        
    private:
        camera m_Camera;
        hittable_list m_HittableList;

        OnHitFunc m_OnHitCallback;
    };
}
