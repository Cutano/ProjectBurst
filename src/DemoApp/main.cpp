#include <memory>

#include "BurstLib.h"

std::shared_ptr<Burst::Scene> g_Scene;

class DemoApp : public Burst::Application
{
public:
    DemoApp(std::shared_ptr<Burst::Scene> scene);
    ~DemoApp();

private:
};

DemoApp::DemoApp(std::shared_ptr<Burst::Scene> scene) : Application(scene)
{
}

DemoApp::~DemoApp()
{
}

Burst::hittable_list random_scene()
{
    Burst::hittable_list world;

    auto ground_material = make_shared<Burst::lambertian>(glm::vec3(0.5, 0.5, 0.5));
    world.add(make_shared<Burst::sphere>(glm::vec3(0, -1000, 0), 1000, ground_material));

    auto material1 = make_shared<Burst::dielectric>(1.5);
    world.add(make_shared<Burst::sphere>(glm::vec3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Burst::lambertian>(glm::vec3(0.4, 0.2, 0.1));
    world.add(make_shared<Burst::sphere>(glm::vec3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Burst::metal>(glm::vec3(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<Burst::sphere>(glm::vec3(4, 1, 0), 1.0, material3));

    return world;
}

glm::vec3 ray_color(
    const Burst::ray& r,
    const glm::vec3& background,
    const Burst::hittable& world,
    shared_ptr<Burst::hittable> lights,
    int depth
)
{
    Burst::hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return glm::vec3(0, 0, 0);

    if (world.hit(r, 0.001, Burst::infinity, rec))
    {
        return {1, 0.8, 0.8};
    }

    glm::vec3 unit_direction = normalize(r.direction());
    auto t = 0.5 * (unit_direction.y + 1.0);
    return (1.0 - t) * glm::vec3(1.0, 1.0, 1.0) + t * glm::vec3(0.5, 0.7, 1.0);
}

int main()
{
    glm::vec3 lookfrom(13, 2, 3);
    glm::vec3 lookat(0, 0, 0);
    glm::vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    Burst::camera cam(lookfrom, lookat, vup, 20, 16.0 / 9.0, aperture, dist_to_focus);

    g_Scene = std::make_shared<Burst::Scene>(cam);
    g_Scene->SetOnHitCallback(ray_color);
    g_Scene->SetHittableList(random_scene());

    const auto app = new DemoApp(g_Scene);

    app->Run();

    delete app;

    return 0;
}
