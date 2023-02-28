#include "Precompiled.h"
#include "Application.h"

#include "Scene/Scene.h"
#include "Engine.h"
#include "Utilities/Timer.h"

#include "Graphics/RHI/Renderer.h"
#include "Graphics/RHI/GraphicsContext.h"
#include "Graphics/Renderers/SceneRenderer.h"
#include "Graphics/Camera/Camera.h"
#include "Graphics/Material.h"
#include "Graphics/Renderers/DebugRenderer.h"
#include "Graphics/Renderers/GridRenderer.h"
#include "Graphics/Font.h"

#include "Maths/Transform.h"

#include "Scene/EntityFactory.h"
#include "Utilities/LoadImage.h"
#include "Core/OS/Input.h"
#include "Core/OS/Window.h"
#include "Core/OS/OS.h"
#include "Core/Profiler.h"
#include "Core/VFS.h"
#include "Core/JobSystem.h"
#include "Core/StringUtilities.h"
#include "Core/OS/FileSystem.h"
#include "Scripting/Lua/LuaManager.h"
#include "ImGui/ImGuiManager.h"
#include "Events/ApplicationEvent.h"
#include "Audio/AudioManager.h"
#include "Audio/Sound.h"
#include "Physics/B2PhysicsEngine/B2PhysicsEngine.h"
#include "Physics/ShapesPhysicsEngine/ShapesPhysicsEngine.h"

#include "Embedded/splash.inl"

#if __has_include(<filesystem>)
#include <filesystem>
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
#endif

#include <cereal/archives/json.hpp>
#include <imgui/imgui.h>

namespace Shapes
{
    Application* Application::s_Instance = nullptr;

    Application::Application()
        : m_Frames(0)
        , m_Updates(0)
        , m_SceneViewWidth(800)
        , m_SceneViewHeight(600)
    {
        SHAPES_PROFILE_FUNCTION();
        SHAPES_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;
    }

    Application::~Application()
    {
        SHAPES_PROFILE_FUNCTION();
        ImGui::DestroyContext();
    }

    void Application::OpenProject(const std::string& filePath)
    {
        SHAPES_PROFILE_FUNCTION();
        m_ProjectSettings.m_ProjectName = StringUtilities::GetFileName(filePath);
        m_ProjectSettings.m_ProjectName = StringUtilities::RemoveFilePathExtension(m_ProjectSettings.m_ProjectName);

#ifndef SHAPES_PLATFORM_IOS
        auto projectRoot                = StringUtilities::GetFileLocation(filePath);
        m_ProjectSettings.m_ProjectRoot = projectRoot;
#endif

        m_SceneManager = CreateUniquePtr<SceneManager>();

        Deserialise();

        m_SceneManager->LoadCurrentList();
        m_SceneManager->ApplySceneSwitch();
    }

    void Application::OpenNewProject(const std::string& path, const std::string& name)
    {
        SHAPES_PROFILE_FUNCTION();
        m_ProjectSettings.m_ProjectRoot = path + name + "/";
        m_ProjectSettings.m_ProjectName = name;

        std::filesystem::create_directory(m_ProjectSettings.m_ProjectRoot);

        m_SceneManager = CreateUniquePtr<SceneManager>();

        MountVFSPaths();
        // Set Default values
        m_ProjectSettings.RenderAPI   = 1;
        m_ProjectSettings.Width       = 1200;
        m_ProjectSettings.Height      = 800;
        m_ProjectSettings.Borderless  = false;
        m_ProjectSettings.VSync       = true;
        m_ProjectSettings.Title       = "App";
        m_ProjectSettings.ShowConsole = false;
        m_ProjectSettings.Fullscreen  = false;

#ifdef SHAPES_PLATFORM_MACOS
        // This is assuming Application in bin/Release-macos-x86_64/ShapesEditor.app
        SHAPES_LOG_INFO(StringUtilities::GetFileLocation(OS::Instance()->GetExecutablePath()));
        m_ProjectSettings.m_EngineAssetPath = StringUtilities::GetFileLocation(OS::Instance()->GetExecutablePath()) + "../../../../../Shapes/Assets/";
#else
        m_ProjectSettings.m_EngineAssetPath = StringUtilities::GetFileLocation(OS::Instance()->GetExecutablePath()) + "../../Shapes/Assets/";
#endif

        VFS::Get().Mount("CoreShaders", m_ProjectSettings.m_EngineAssetPath + std::string("Shaders"));

        if(!FileSystem::FolderExists(m_ProjectSettings.m_ProjectRoot + "Assets"))
            std::filesystem::create_directory(m_ProjectSettings.m_ProjectRoot + "Assets");

        if(!FileSystem::FolderExists(m_ProjectSettings.m_ProjectRoot + "Assets/Scripts"))
            std::filesystem::create_directory(m_ProjectSettings.m_ProjectRoot + "Assets/Scripts");

        if(!FileSystem::FolderExists(m_ProjectSettings.m_ProjectRoot + "Assets/Scenes"))
            std::filesystem::create_directory(m_ProjectSettings.m_ProjectRoot + "Assets/Scenes");

        if(!FileSystem::FolderExists(m_ProjectSettings.m_ProjectRoot + "Assets/Textures"))
            std::filesystem::create_directory(m_ProjectSettings.m_ProjectRoot + "Assets/Textures");

        if(!FileSystem::FolderExists(m_ProjectSettings.m_ProjectRoot + "Assets/Meshes"))
            std::filesystem::create_directory(m_ProjectSettings.m_ProjectRoot + "Assets/Meshes");

        if(!FileSystem::FolderExists(m_ProjectSettings.m_ProjectRoot + "Assets/Sounds"))
            std::filesystem::create_directory(m_ProjectSettings.m_ProjectRoot + "Assets/Sounds");

        MountVFSPaths();

        m_SceneManager->EnqueueScene(new Scene("Empty Scene"));
        m_SceneManager->SwitchScene(0);

        // Set Default values
        m_ProjectSettings.Title      = "App";
        m_ProjectSettings.Fullscreen = false;

        m_SceneManager->ApplySceneSwitch();

        m_ProjectLoaded = true;

        Serialise();
    }

    void Application::MountVFSPaths()
    {
        VFS::Get().Mount("Meshes", m_ProjectSettings.m_ProjectRoot + std::string("Assets/Meshes"));
        VFS::Get().Mount("Textures", m_ProjectSettings.m_ProjectRoot + std::string("Assets/Textures"));
        VFS::Get().Mount("Sounds", m_ProjectSettings.m_ProjectRoot + std::string("Assets/Sounds"));
        VFS::Get().Mount("Scripts", m_ProjectSettings.m_ProjectRoot + std::string("Assets/Scripts"));
        VFS::Get().Mount("Scenes", m_ProjectSettings.m_ProjectRoot + std::string("Assets/Scenes"));
        VFS::Get().Mount("Assets", m_ProjectSettings.m_ProjectRoot + std::string("Assets"));
    }

    Scene* Application::GetCurrentScene() const
    {
        SHAPES_PROFILE_FUNCTION();
        return m_SceneManager->GetCurrentScene();
    }

    void Application::Init()
    {
        SHAPES_PROFILE_FUNCTION();
        m_SceneManager = CreateUniquePtr<SceneManager>();
        Deserialise();

        Engine::Get();
        LuaManager::Get().OnInit();

        m_Timer = CreateUniquePtr<Timer>();

        Graphics::GraphicsContext::SetRenderAPI(static_cast<Graphics::RenderAPI>(m_ProjectSettings.RenderAPI));

        WindowDesc windowDesc;
        windowDesc.Width       = m_ProjectSettings.Width;
        windowDesc.Height      = m_ProjectSettings.Height;
        windowDesc.RenderAPI   = m_ProjectSettings.RenderAPI;
        windowDesc.Fullscreen  = m_ProjectSettings.Fullscreen;
        windowDesc.Borderless  = m_ProjectSettings.Borderless;
        windowDesc.ShowConsole = m_ProjectSettings.ShowConsole;
        windowDesc.Title       = m_ProjectSettings.Title;
        windowDesc.VSync       = m_ProjectSettings.VSync;

        // Initialise the Window
        m_Window = UniquePtr<Window>(Window::Create(windowDesc));
        if(!m_Window->HasInitialised())
            OnQuit();

        m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

        m_EditorState = EditorState::Play;

        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        m_ShaderLibrary = CreateSharedPtr<ShaderLibrary>();
        m_ModelLibrary  = CreateSharedPtr<ModelLibrary>();
        m_FontLibrary   = CreateSharedPtr<FontLibrary>();

        bool loadEmbeddedShaders = true;
        if(FileSystem::FolderExists(m_ProjectSettings.m_EngineAssetPath + "Shaders"))
            loadEmbeddedShaders = false;

        Graphics::Renderer::Init(false);

        if(m_ProjectSettings.Fullscreen)
            m_Window->Maximise();

        // Draw Splash Screeh
        {
            auto splashTexture = Graphics::Texture2D::CreateFromSource(splashWidth, splashHeight, (void*)splash);
            Graphics::Renderer::GetRenderer()->Begin();
            Graphics::Renderer::GetRenderer()->DrawSplashScreen(splashTexture);
            Graphics::Renderer::GetRenderer()->Present();
            // To Display the window
            m_Window->ProcessInput();
            m_Window->OnUpdate();

            delete splashTexture;
        }

        uint32_t screenWidth  = m_Window->GetWidth();
        uint32_t screenHeight = m_Window->GetHeight();
        m_SystemManager       = CreateUniquePtr<SystemManager>();

        System::JobSystem::Context context;

        System::JobSystem::Execute(context, [](JobDispatchArgs args)
                                   { Shapes::Input::Get(); });

        System::JobSystem::Execute(context, [this](JobDispatchArgs args)
                                   {
                auto audioManager = AudioManager::Create();
                if(audioManager)
                {
                    audioManager->OnInit();
                    audioManager->SetPaused(true);
                    m_SystemManager->RegisterSystem<AudioManager>(audioManager);
                } });

        System::JobSystem::Execute(context, [this](JobDispatchArgs args)
                                   {
                m_SystemManager->RegisterSystem<ShapesPhysicsEngine>();
                m_SystemManager->RegisterSystem<B2PhysicsEngine>(); });

        System::JobSystem::Execute(context, [this](JobDispatchArgs args)
                                   { m_SceneManager->LoadCurrentList(); });

        m_ImGuiManager = CreateUniquePtr<ImGuiManager>(false);
        m_ImGuiManager->OnInit();
        SHAPES_LOG_INFO("Initialised ImGui Manager");

        m_SceneRenderer = CreateUniquePtr<Graphics::SceneRenderer>(screenWidth, screenHeight);

        m_CurrentState = AppState::Running;
        System::JobSystem::Wait(context);

        Graphics::Material::InitDefaultTexture();
        Graphics::Font::InitDefaultFont();
        m_SceneRenderer->EnableDebugRenderer(true);
    }

    void Application::OnQuit()
    {
        SHAPES_PROFILE_FUNCTION();
        Serialise();
        Graphics::Material::ReleaseDefaultTexture();
        Graphics::Font::ShutdownDefaultFont();
        Engine::Release();
        Input::Release();

        m_ShaderLibrary.reset();
        m_ModelLibrary.reset();
        m_FontLibrary.reset();
        m_SceneManager.reset();
        m_SceneRenderer.reset();
        m_SystemManager.reset();
        m_ImGuiManager.reset();
        LuaManager::Release();

        Graphics::Pipeline::ClearCache();
        Graphics::RenderPass::ClearCache();
        Graphics::Framebuffer::ClearCache();
        Graphics::Renderer::Release();

        m_Window.reset();
    }

    glm::vec2 Application::GetWindowSize() const
    {
        if(!m_Window)
            return glm::vec2(0.0f, 0.0f);
        return glm::vec2(static_cast<float>(m_Window->GetWidth()), static_cast<float>(m_Window->GetHeight()));
    }

    float Application::GetWindowDPI() const
    {
        if(!m_Window)
            return 1.0f;

        return m_Window->GetDPIScale();
    }

    bool Application::OnFrame()
    {
        SHAPES_PROFILE_FUNCTION();
        if(m_SceneManager->GetSwitchingScene())
        {
            SHAPES_PROFILE_SCOPE("Application::SceneSwitch");
            Graphics::Renderer::GetGraphicsContext()->WaitIdle();
            m_SceneManager->ApplySceneSwitch();
            return m_CurrentState != AppState::Closing;
        }

        double now  = m_Timer->GetElapsedSD();
        auto& stats = Engine::Get().Statistics();
        auto& ts    = Engine::GetTimeStep();

        if(ts.GetSeconds() > 5)
        {
            SHAPES_LOG_WARN("Large frame time {0}", ts.GetSeconds());
#ifdef SHAPES_DISABLE_LARGE_FRAME_TIME
            // Added to stop application locking computer
            // Exit if frametime exceeds 5 seconds
            return false;
#endif
        }
        {
            SHAPES_PROFILE_SCOPE("Application::TimeStepUpdates");
            ts.OnUpdate();

            ImGuiIO& io  = ImGui::GetIO();
            io.DeltaTime = (float)ts.GetSeconds();

            stats.FrameTime = ts.GetMillis();
        }

        Input::Get().ResetPressed();
        m_Window->ProcessInput();

        {
            std::scoped_lock<std::mutex> lock(m_EventQueueMutex);

            // Process custom event queue
            while(m_EventQueue.size() > 0)
            {
                auto& func = m_EventQueue.front();
                func();
                m_EventQueue.pop();
            }
        }

        {
            SHAPES_PROFILE_SCOPE("Application::ImGui::NewFrame");
            ImGui::NewFrame();
        }

        {
            SHAPES_PROFILE_SCOPE("Application::Update");
            OnUpdate(ts);
            m_Updates++;
        }

        // Exit frame early if escape or close button clicked
        // Prevents a crash with vulkan/moltenvk
        if(m_CurrentState == AppState::Closing)
            return false;

        std::thread updateThread = std::thread(Application::UpdateSystems);

        if(!m_Minimized)
        {
            SHAPES_PROFILE_SCOPE("Application::Render");

            Graphics::Renderer::GetRenderer()->Begin();

            OnRender();
            m_ImGuiManager->OnRender(m_SceneManager->GetCurrentScene());

            Graphics::Renderer::GetRenderer()->Present();

            Graphics::Pipeline::DeleteUnusedCache();
            Graphics::Framebuffer::DeleteUnusedCache();
            Graphics::RenderPass::DeleteUnusedCache();

            // m_ShaderLibrary->Update(ts.GetElapsedSeconds());
            m_ModelLibrary->Update((float)ts.GetElapsedSeconds());
            m_FontLibrary->Update((float)ts.GetElapsedSeconds());
            m_Frames++;
        }
        else
        {
            ImGui::Render();
        }

        {
            SHAPES_PROFILE_SCOPE("Application::UpdateGraphicsStats");
            stats.UsedGPUMemory  = Graphics::Renderer::GetGraphicsContext()->GetGPUMemoryUsed();
            stats.TotalGPUMemory = Graphics::Renderer::GetGraphicsContext()->GetTotalGPUMemory();
        }

        {
            SHAPES_PROFILE_SCOPE("Application::WindowUpdate");
            m_Window->UpdateCursorImGui();
            m_Window->OnUpdate();
        }

        {
            SHAPES_PROFILE_SCOPE("Wait System update");
            updateThread.join();

            m_SystemManager->GetSystem<ShapesPhysicsEngine>()->SyncTransforms(m_SceneManager->GetCurrentScene());
            m_SystemManager->GetSystem<B2PhysicsEngine>()->SyncTransforms(m_SceneManager->GetCurrentScene());
        }

        if(now - m_SecondTimer > 1.0f)
        {
            SHAPES_PROFILE_SCOPE("Application::FrameRateCalc");
            m_SecondTimer += 1.0f;

            stats.FramesPerSecond  = m_Frames;
            stats.UpdatesPerSecond = m_Updates;

            m_Frames  = 0;
            m_Updates = 0;
        }

        SHAPES_PROFILE_FRAMEMARKER();

        return m_CurrentState != AppState::Closing;
    }

    void Application::OnRender()
    {
        SHAPES_PROFILE_FUNCTION();
        if(!m_SceneManager->GetCurrentScene())
            return;

        if(!m_DisableMainSceneRenderer)
        {
            m_SceneRenderer->BeginScene(m_SceneManager->GetCurrentScene());
            m_SceneRenderer->OnRender();

            // Clears debug line and point lists
            DebugRenderer::Reset();
            OnDebugDraw();
        }
    }

    void Application::OnDebugDraw()
    {
        m_SystemManager->OnDebugDraw();
    }

    void Application::OnUpdate(const TimeStep& dt)
    {
        SHAPES_PROFILE_FUNCTION();
        if(!m_SceneManager->GetCurrentScene())
            return;

        if(Application::Get().GetEditorState() != EditorState::Paused
           && Application::Get().GetEditorState() != EditorState::Preview)
        {
            LuaManager::Get().OnUpdate(m_SceneManager->GetCurrentScene());
            m_SceneManager->GetCurrentScene()->OnUpdate(dt);
        }
        m_ImGuiManager->OnUpdate(dt, m_SceneManager->GetCurrentScene());
    }

    void Application::OnEvent(Event& e)
    {
        SHAPES_PROFILE_FUNCTION();
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

        if(m_ImGuiManager)
            m_ImGuiManager->OnEvent(e);
        if(e.Handled())
            return;

        if(m_SceneRenderer)
            m_SceneRenderer->OnEvent(e);

        if(e.Handled())
            return;

        if(m_SceneManager->GetCurrentScene())
            m_SceneManager->GetCurrentScene()->OnEvent(e);

        Input::Get().OnEvent(e);
    }

    void Application::Run()
    {
        while(OnFrame())
        {
        }

        OnQuit();
    }

    void Application::OnNewScene(Scene* scene)
    {
        SHAPES_PROFILE_FUNCTION();
        m_SceneRenderer->OnNewScene(scene);
    }

    SharedPtr<ShaderLibrary>& Application::GetShaderLibrary()
    {
        return m_ShaderLibrary;
    }

    SharedPtr<ModelLibrary>& Application::GetModelLibrary()
    {
        return m_ModelLibrary;
    }

    SharedPtr<FontLibrary>& Application::GetFontLibrary()
    {
        return m_FontLibrary;
    }

    void Application::OnExitScene()
    {
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_CurrentState = AppState::Closing;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        SHAPES_PROFILE_FUNCTION();
        Graphics::Renderer::GetGraphicsContext()->WaitIdle();

        int width = e.GetWidth(), height = e.GetHeight();

        if(width == 0 || height == 0)
        {
            m_Minimized = true;
            return false;
        }
        m_Minimized = false;

        Graphics::Renderer::GetRenderer()->OnResize(width, height);

        if(m_SceneRenderer)
            m_SceneRenderer->OnResize(width, height);

        Graphics::Renderer::GetGraphicsContext()->WaitIdle();

        return false;
    }

    void Application::OnImGui()
    {
        SHAPES_PROFILE_FUNCTION();
        if(!m_SceneManager->GetCurrentScene())
            return;

        m_SceneManager->GetCurrentScene()->OnImGui();
    }

    void Application::UpdateSystems()
    {
        SHAPES_PROFILE_FUNCTION();
        if(Application::Get().GetEditorState() != EditorState::Paused
           && Application::Get().GetEditorState() != EditorState::Preview)
        {
            auto scene = Application::Get().GetSceneManager()->GetCurrentScene();

            if(!scene)
                return;

            Application::Get().GetSystemManager()->OnUpdate(Engine::GetTimeStep(), scene);
        }
    }

    void Application::OnSceneViewSizeUpdated(uint32_t width, uint32_t height)
    {
        SHAPES_PROFILE_FUNCTION();
        Graphics::Renderer::GetGraphicsContext()->WaitIdle();

        WindowResizeEvent e(width, height);
        if(width == 0 || height == 0)
        {
            m_Minimized = true;
            return;
        }
        m_Minimized = false;
        m_SceneRenderer->OnResize(width, height);
        m_SceneRenderer->OnEvent(e);

        Graphics::Renderer::GetGraphicsContext()->WaitIdle();
    }

    void Application::Serialise()
    {
        SHAPES_PROFILE_FUNCTION();
        {
            std::stringstream storage;
            {
                // output finishes flushing its contents when it goes out of scope
                cereal::JSONOutputArchive output { storage };
                output(*this);
            }
            auto fullPath = m_ProjectSettings.m_ProjectRoot + m_ProjectSettings.m_ProjectName + std::string(".lmproj");
            SHAPES_LOG_INFO("Serialising Application {0}", fullPath);
            FileSystem::WriteTextFile(fullPath, storage.str());
        }
    }

    void Application::Deserialise()
    {
        SHAPES_PROFILE_FUNCTION();
        {
            auto filePath = m_ProjectSettings.m_ProjectRoot + m_ProjectSettings.m_ProjectName + std::string(".lmproj");

            MountVFSPaths();

            if(!FileSystem::FileExists(filePath))
            {
                SHAPES_LOG_INFO("No saved Project file found {0}", filePath);
                {
                    m_SceneManager = CreateUniquePtr<SceneManager>();

                    // Set Default values
                    m_ProjectSettings.RenderAPI   = 1;
                    m_ProjectSettings.Width       = 1200;
                    m_ProjectSettings.Height      = 800;
                    m_ProjectSettings.Borderless  = false;
                    m_ProjectSettings.VSync       = true;
                    m_ProjectSettings.Title       = "App";
                    m_ProjectSettings.ShowConsole = false;
                    m_ProjectSettings.Fullscreen  = false;

                    m_ProjectLoaded = false;

#ifdef SHAPES_PLATFORM_MACOS
                    // This is assuming Application in bin/Release-macos-x86_64/ShapesEditor.app
                    SHAPES_LOG_INFO(StringUtilities::GetFileLocation(OS::Instance()->GetExecutablePath()));
                    m_ProjectSettings.m_EngineAssetPath = StringUtilities::GetFileLocation(OS::Instance()->GetExecutablePath()) + "../../../../../Shapes/Assets/";

                    if(!FileSystem::FolderExists(m_ProjectSettings.m_EngineAssetPath))
                    {
                        m_ProjectSettings.m_EngineAssetPath = StringUtilities::GetFileLocation(OS::Instance()->GetExecutablePath()) + "../../Shapes/Assets/";
                    }
#else
                    m_ProjectSettings.m_EngineAssetPath = StringUtilities::GetFileLocation(OS::Instance()->GetExecutablePath()) + "../../Shapes/Assets/";
#endif
                    VFS::Get().Mount("CoreShaders", m_ProjectSettings.m_EngineAssetPath + std::string("Shaders"));

                    m_SceneManager->EnqueueScene(new Scene("Empty Scene"));
                    m_SceneManager->SwitchScene(0);
                }
                return;
            }

            m_ProjectLoaded = true;

            std::string data = FileSystem::ReadTextFile(filePath);
            std::istringstream istr;
            istr.str(data);
            try
            {
                cereal::JSONInputArchive input(istr);
                input(*this);
            }
            catch(...)
            {
                // Set Default values
                m_ProjectSettings.RenderAPI   = 1;
                m_ProjectSettings.Width       = 1200;
                m_ProjectSettings.Height      = 800;
                m_ProjectSettings.Borderless  = false;
                m_ProjectSettings.VSync       = true;
                m_ProjectSettings.Title       = "App";
                m_ProjectSettings.ShowConsole = false;
                m_ProjectSettings.Fullscreen  = false;

#ifdef SHAPES_PLATFORM_MACOS
                m_ProjectSettings.m_EngineAssetPath = StringUtilities::GetFileLocation(OS::Instance()->GetExecutablePath()) + "../../../../../Shapes/Assets/";
#else
                m_ProjectSettings.m_EngineAssetPath = StringUtilities::GetFileLocation(OS::Instance()->GetExecutablePath()) + "../../Shapes/Assets/";
#endif

                VFS::Get().Mount("CoreShaders", m_ProjectSettings.m_EngineAssetPath + std::string("Shaders"));

                m_SceneManager->EnqueueScene(new Scene("Empty Scene"));
                m_SceneManager->SwitchScene(0);

                SHAPES_LOG_ERROR("Failed to load project");
            }
        }
    }
}
