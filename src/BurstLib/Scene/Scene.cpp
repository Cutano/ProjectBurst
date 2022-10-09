#include "Scene.h"

namespace Burst
{
    Scene::Scene()
    {
    }

    Scene::Scene(const camera camera) : m_Camera(camera)
    {
    }

    Scene::~Scene()
    {
    }

    camera& Scene::GetCamera()
    {
        return m_Camera;
    }

    void Scene::SetCamera(const camera& m_camera)
    {
        m_Camera = m_camera;
    }

    const OnHitFunc& Scene::GetOnHitCallback() const
    {
        return m_OnHitCallback;
    }

    void Scene::SetOnHitCallback(OnHitFunc onHitCallback)
    {
        m_OnHitCallback = std::move(onHitCallback);
    }

    const hittable_list& Scene::GetHittableList() const
    {
        return m_HittableList;
    }

    void Scene::SetHittableList(hittable_list HittableList)
    {
        m_HittableList = std::move(HittableList);
    }
}
