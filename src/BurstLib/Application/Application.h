#pragma once

#include <memory>
#include <string>

struct GLFWwindow;

namespace Burst
{
    class Scene;
    class Pixel;
    class Renderer;

    class Application
    {
    public:
        Application(std::shared_ptr<Scene> scene);
        virtual ~Application();

        void Run();

    protected:
        virtual void Init() {}
        virtual void Shutdown() {}

        std::shared_ptr<Scene> m_Scene;
        std::shared_ptr<Renderer> m_Renderer;
        GLFWwindow *m_Window;

    private:
        int m_ImageWidth{1440};
        int m_ImageHeight{900};
        int m_ImageSettingWidth{1440};
        int m_ImageSettingHeight{900};

        bool m_ShowDemoWindow{true};
        bool m_ShowPreviewWindow{true};
        bool m_ShowSettingWindow{true};

        unsigned int m_PreviewTexID;

        void DrawDockSpace();
        void DrawPreviewWindow();
        void DrawSettingWindow();

        void RenderCallback(float progressUNORM, Pixel* image);
    };
}