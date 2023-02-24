#include <ShapesEngine.h>
#include <Shapes/Core/EntryPoint.h>

using namespace Shapes;

class Runtime : public Application
{
	friend class Application;
public:
	explicit Runtime()
		: Application()
	{
		SHAPES_PROFILE_FUNCTION();
	}

	~Runtime()
	{
	}

	void OnEvent(Event& e) override
	{
		Application::OnEvent(e);

		if (Input::Get().GetKeyPressed(Shapes::InputCode::Key::Escape))
			Application::SetAppState(AppState::Closing);
	}

    void Init() override
    {
        std::vector<std::string> projectLocations = {
            "../ExampleProject/Example.lmproj",
            //OS::Instance()->GetExecutablePath() + "/ExampleProject/Example.lmproj",
        };

        bool fileFound = false;
        std::string filePath;
        for (auto& path : projectLocations)
        {
            if (FileSystem::FileExists(path))
            {
                SHAPES_LOG_INFO("Loaded Project {0}", path);
                m_ProjectSettings.m_ProjectRoot = StringUtilities::GetFileLocation(path);
                m_ProjectSettings.m_ProjectName = "Example";
                break;
            }
        }

        Application::Init();
        Application::SetEditorState(EditorState::Play);
        Application::Get().GetWindow()->SetWindowTitle("Runtime");
        Application::Get().GetWindow()->SetEventCallback(BIND_EVENT_FN(Runtime::OnEvent));
    }

    void OnImGui() override
    {
        ImGui::Begin("Metrics");
        ImGui::Text("FPS : %.2f", (float)Shapes::Engine::Get().Statistics().FramesPerSecond);
        ImGui::End();
        Application::OnImGui();
    }
};

Shapes::Application* Shapes::CreateApplication()
{
	return new ::Runtime();
}
