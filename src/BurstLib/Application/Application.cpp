#include "Application.h"

#include <BS_thread_pool.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "Renderer/Renderer.h"

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

namespace Burst
{
    Application::Application(std::shared_ptr<Scene> scene) : m_Scene(scene)
    {
        // Setup window
        glfwSetErrorCallback(glfw_error_callback);

        glfwInit();

        const char *glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_Window = glfwCreateWindow(1440, 900, "Burst Renderer", NULL, NULL);

        glfwMakeContextCurrent(m_Window);
        glfwSwapInterval(1); // Enable vsync

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            spdlog::error("Failed init opengl!");
        }
        

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
        // io.ConfigViewportsNoAutoMerge = true;
        // io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsLight();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle &style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        m_Renderer = std::make_shared<Renderer>();

        glGenTextures(1, &m_PreviewTexID);

        glBindTexture(GL_TEXTURE_2D, m_PreviewTexID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Application::~Application()
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Application::Run()
    {
        while (!glfwWindowShouldClose(m_Window))
        {
            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            glfwWaitEvents();

            if (m_IsGenerating)
            {
                glBindTexture(GL_TEXTURE_2D, m_PreviewTexID);
        
                // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_ImageWidth, m_ImageHeight, GL_RGB, GL_UNSIGNED_BYTE, m_ImageData);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_ImageWidth, m_ImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, m_ImageData);
                glCheckError();
            
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            if (m_Progress >= 0.999999999999f)
            {
                m_IsGenerating = false;
            }

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

            DrawDockSpace();

            // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
            if (m_ShowDemoWindow)
                ImGui::ShowDemoWindow(&m_ShowDemoWindow);

            if (m_ShowPreviewWindow)
                DrawPreviewWindow();

            if (m_ShowSettingWindow)
                DrawSettingWindow();

            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(m_Window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Update and Render additional Platform Windows
            // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
            //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
            ImGuiIO &io = ImGui::GetIO();
            (void)io;
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow *backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }

            glfwSwapBuffers(m_Window);
        }
    }

    void Application::DrawDockSpace()
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        ImGui::Begin("DockSpaceWindow", nullptr, windowFlags);
        ImGui::PopStyleVar(3);
        const ImGuiID dockSpaceId = ImGui::GetID("DockSpace");
        ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f));

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Generate", "Ctrl+G"))
                {
                    m_ImageWidth = m_ImageSettingWidth;
                    m_ImageHeight = m_ImageSettingHeight;
                    m_ImageData = nullptr;
                    m_Progress = 0;
                    m_IsGenerating = true;

                    glBindTexture(GL_TEXTURE_2D, m_PreviewTexID);
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
                    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_ImageWidth, m_ImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
                    glCheckError();
                    glBindTexture(GL_TEXTURE_2D, 0);
                    
                    auto future = m_Renderer->Render(m_ImageSettingWidth, m_ImageSettingHeight, m_Scene, [this](float prog, Pixel* img) { RenderCallback(prog, img); });
                }

                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {
                    m_Renderer->SaveImage("./img.png");
                }
                
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Window"))
            {
                ImGui::MenuItem("Show Demo", nullptr, &m_ShowDemoWindow);
                ImGui::MenuItem("Show Preview", nullptr, &m_ShowPreviewWindow);
                ImGui::MenuItem("Show Setting", nullptr, &m_ShowSettingWindow);
                ImGui::EndMenu();
            }

            ImGui::ProgressBar(m_Progress);
            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    void Application::DrawPreviewWindow()
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_HorizontalScrollbar;
        ImGui::Begin("Preview", &m_ShowPreviewWindow, windowFlags);
        
        ImGui::Image((void*)(intptr_t)m_PreviewTexID, ImVec2(m_ImageWidth, m_ImageHeight));

        ImGui::End();
    }

    void Application::DrawSettingWindow()
    {
        ImGuiWindowFlags windowFlags = 0;
        ImGui::Begin("Setting", &m_ShowSettingWindow, windowFlags);
        ImGui::DragInt("Image Width", &m_ImageSettingWidth, 1.0f, 100, 1e5);
        ImGui::DragInt("Image Height", &m_ImageSettingHeight, 1.0f, 100, 1e5);
        ImGui::End();
    }

    void Application::RenderCallback(float progressUNORM, Pixel* image)
    {
        // glBindTexture(GL_TEXTURE_2D, m_PreviewTexID);
        
        // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_ImageWidth, m_ImageHeight, GL_RGB, GL_UNSIGNED_BYTE, image);
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_ImageWidth, m_ImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<unsigned char*>(image));
        // glCheckError();

        m_ImageData = reinterpret_cast<unsigned char*>(image);
        m_Progress = progressUNORM;

        glfwPostEmptyEvent();

        spdlog::info("Progress: {}", progressUNORM);
    }
}