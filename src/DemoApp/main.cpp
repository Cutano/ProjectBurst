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

int main()
{
    g_Scene = std::make_shared<Burst::Scene>();

    auto app = new DemoApp(g_Scene);

    app->Run();

    delete app;

    return 0;
}
