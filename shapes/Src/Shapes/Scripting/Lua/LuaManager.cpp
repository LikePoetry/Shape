#include "Precompiled.h"
#include "LuaManager.h"
#include "Maths/Transform.h"
#include "Core/OS/Window.h"
#include "Core/VFS.h"
#include "Scene/Scene.h"
#include "Core/Application.h"
#include "Core/Engine.h"
#include "Core/OS/Input.h"
#include "Scene/SceneManager.h"
#include "LuaScriptComponent.h"
#include "Scene/SceneGraph.h"
#include "Graphics/Camera/ThirdPersonCamera.h"

#include "Scene/Component/Components.h"
#include "Graphics/Camera/Camera.h"
#include "Graphics/Camera/Camera2D.h"

#include "Graphics/Sprite.h"
#include "Graphics/Light.h"
#include "Graphics/RHI/Texture.h"
#include "Graphics/Model.h"
#include "Maths/Random.h"
#include "Scene/Entity.h"
#include "Scene/EntityManager.h"
#include "Scene/EntityFactory.h"
#include "Physics/ShapesPhysicsEngine/ShapesPhysicsEngine.h"

#include "ImGuiLua.h"
#include "PhysicsLua.h"
#include "MathsLua.h"

#include <imgui/imgui.h>
#include <Tracy/TracyLua.hpp>
#include <sol/sol.hpp>

#ifdef CUSTOM_SMART_PTR
namespace sol
{
    template <typename T>
    struct unique_usertype_traits<Shapes::SharedPtr<T>>
    {
        typedef T type;
        typedef Shapes::SharedPtr<T> actual_type;
        static const bool value = true;

        static bool is_null(const actual_type& ptr)
        {
            return ptr == nullptr;
        }

        static type* get(const actual_type& ptr)
        {
            return ptr.get();
        }
    };

    template <typename T>
    struct unique_usertype_traits<Shapes::UniquePtr<T>>
    {
        typedef T type;
        typedef Shapes::UniquePtr<T> actual_type;
        static const bool value = true;

        static bool is_null(const actual_type& ptr)
        {
            return ptr == nullptr;
        }

        static type* get(const actual_type& ptr)
        {
            return ptr.get();
        }
    };

    template <typename T>
    struct unique_usertype_traits<Shapes::WeakPtr<T>>
    {
        typedef T type;
        typedef Shapes::WeakPtr<T> actual_type;
        static const bool value = true;

        static bool is_null(const actual_type& ptr)
        {
            return ptr == nullptr;
        }

        static type* get(const actual_type& ptr)
        {
            return ptr.get();
        }
    };
}

#endif

namespace Shapes
{

    std::vector<std::string> LuaManager::s_Identifiers = {
        "Log",
        "Trace",
        "Info",
        "Warn",
        "Error",
        "Critical",
        "Input",
        "GetKeyPressed",
        "GetKeyHeld",
        "GetMouseClicked",
        "GetMouseHeld",
        "GetMousePosition",
        "GetScrollOffset",
        "enttRegistry",
        "Entity",
        "EntityManager",
        "Create"
        "GetRegistry",
        "Valid",
        "Destroy",
        "SetParent",
        "GetParent",
        "IsParent",
        "GetChildren",
        "SetActive",
        "Active",
        "GetEntityByName",
        "AddPyramidEntity",
        "AddSphereEntity",
        "AddLightCubeEntity",
        "NameComponent",
        "GetNameComponent",
        "GetCurrentEntity",
        "SetThisComponent",
        "LuaScriptComponent",
        "GetLuaScriptComponent",
        "Transform",
        "GetTransform"
    };

    LuaManager::LuaManager()
        : m_State(nullptr)
    {
    }

    void LuaManager::OnInit()
    {
        SHAPES_PROFILE_FUNCTION();

        m_State = new sol::state();
        m_State->open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table);
        tracy::LuaRegister(m_State->lua_state());

        BindAppLua(*m_State);
        BindInputLua(*m_State);
        BindMathsLua(*m_State);
        BindImGuiLua(*m_State);
        BindECSLua(*m_State);
        BindLogLua(*m_State);
        BindSceneLua(*m_State);
        BindPhysicsLua(*m_State);
    }

    LuaManager::~LuaManager()
    {
        delete m_State;
    }

    void LuaManager::OnInit(Scene* scene)
    {
        SHAPES_PROFILE_FUNCTION();
        auto& registry = scene->GetRegistry();

        auto view = registry.view<LuaScriptComponent>();

        if(view.empty())
            return;

        for(auto entity : view)
        {
            auto& luaScript = registry.get<LuaScriptComponent>(entity);
            luaScript.SetThisComponent();
            luaScript.OnInit();
        }
    }

    void LuaManager::OnUpdate(Scene* scene)
    {
        SHAPES_PROFILE_FUNCTION();
        auto& registry = scene->GetRegistry();

        auto view = registry.view<LuaScriptComponent>();

        if(view.empty())
            return;

        float dt = (float)Engine::Get().GetTimeStep().GetSeconds();

        for(auto entity : view)
        {
            auto& luaScript = registry.get<LuaScriptComponent>(entity);
            luaScript.OnUpdate(dt);
        }
    }

    entt::entity GetEntityByName(entt::registry& registry, const std::string& name)
    {
        SHAPES_PROFILE_FUNCTION();
        entt::entity e = entt::null;
        registry.view<NameComponent>().each([&](const entt::entity& entity, const NameComponent& component)
                                            {
                if(name == component.name)
                {
                    e = entity;
                } });

        return e;
    }

    void LuaManager::BindLogLua(sol::state& state)
    {
        SHAPES_PROFILE_FUNCTION();
        auto log = state.create_table("Log");

        log.set_function("Trace", [&](sol::this_state s, std::string_view message)
                         { SHAPES_LOG_TRACE(message); });

        log.set_function("Info", [&](sol::this_state s, std::string_view message)
                         { SHAPES_LOG_TRACE(message); });

        log.set_function("Warn", [&](sol::this_state s, std::string_view message)
                         { SHAPES_LOG_WARN(message); });

        log.set_function("Error", [&](sol::this_state s, std::string_view message)
                         { SHAPES_LOG_ERROR(message); });

        log.set_function("Critical", [&](sol::this_state s, std::string_view message)
                         { SHAPES_LOG_CRITICAL(message); });
    }

    void LuaManager::BindInputLua(sol::state& state)
    {
        SHAPES_PROFILE_FUNCTION();
        auto input = state["Input"].get_or_create<sol::table>();

        input.set_function("GetKeyPressed", [](Shapes::InputCode::Key key) -> bool
                           { return Input::Get().GetKeyPressed(key); });

        input.set_function("GetKeyHeld", [](Shapes::InputCode::Key key) -> bool
                           { return Input::Get().GetKeyHeld(key); });

        input.set_function("GetMouseClicked", [](Shapes::InputCode::MouseKey key) -> bool
                           { return Input::Get().GetMouseClicked(key); });

        input.set_function("GetMouseHeld", [](Shapes::InputCode::MouseKey key) -> bool
                           { return Input::Get().GetMouseHeld(key); });

        input.set_function("GetMousePosition", []() -> glm::vec2
                           { return Input::Get().Get().GetMousePosition(); });

        input.set_function("GetScrollOffset", []() -> float
                           { return Input::Get().Get().GetScrollOffset(); });

        input.set_function("GetControllerAxis", [](int id, int axis) -> float
                           { return Input::Get().GetControllerAxis(id, axis); });

        input.set_function("GetControllerName", [](int id) -> std::string
                           { return Input::Get().GetControllerName(id); });

        input.set_function("GetControllerHat", [](int id, int hat) -> int
                           { return Input::Get().GetControllerHat(id, hat); });

        input.set_function("IsControllerButtonPressed", [](int id, int button) -> bool
                           { return Input::Get().IsControllerButtonPressed(id, button); });

        std::initializer_list<std::pair<sol::string_view, Shapes::InputCode::Key>> keyItems = {
            { "A", Shapes::InputCode::Key::A },
            { "B", Shapes::InputCode::Key::B },
            { "C", Shapes::InputCode::Key::C },
            { "D", Shapes::InputCode::Key::D },
            { "E", Shapes::InputCode::Key::E },
            { "F", Shapes::InputCode::Key::F },
            { "H", Shapes::InputCode::Key::G },
            { "G", Shapes::InputCode::Key::H },
            { "I", Shapes::InputCode::Key::I },
            { "J", Shapes::InputCode::Key::J },
            { "K", Shapes::InputCode::Key::K },
            { "L", Shapes::InputCode::Key::L },
            { "M", Shapes::InputCode::Key::M },
            { "N", Shapes::InputCode::Key::N },
            { "O", Shapes::InputCode::Key::O },
            { "P", Shapes::InputCode::Key::P },
            { "Q", Shapes::InputCode::Key::Q },
            { "R", Shapes::InputCode::Key::R },
            { "S", Shapes::InputCode::Key::S },
            { "T", Shapes::InputCode::Key::T },
            { "U", Shapes::InputCode::Key::U },
            { "V", Shapes::InputCode::Key::V },
            { "W", Shapes::InputCode::Key::W },
            { "X", Shapes::InputCode::Key::X },
            { "Y", Shapes::InputCode::Key::Y },
            { "Z", Shapes::InputCode::Key::Z },
            //{ "UNKOWN", Shapes::InputCode::Key::Unknown },
            { "Space", Shapes::InputCode::Key::Space },
            { "Escape", Shapes::InputCode::Key::Escape },
            { "APOSTROPHE", Shapes::InputCode::Key::Apostrophe },
            { "Comma", Shapes::InputCode::Key::Comma },
            { "MINUS", Shapes::InputCode::Key::Minus },
            { "PERIOD", Shapes::InputCode::Key::Period },
            { "SLASH", Shapes::InputCode::Key::Slash },
            { "SEMICOLON", Shapes::InputCode::Key::Semicolon },
            { "EQUAL", Shapes::InputCode::Key::Equal },
            { "LEFT_BRACKET", Shapes::InputCode::Key::LeftBracket },
            { "BACKSLASH", Shapes::InputCode::Key::Backslash },
            { "RIGHT_BRACKET", Shapes::InputCode::Key::RightBracket },
            //{ "BACK_TICK", Shapes::InputCode::Key::BackTick },
            { "Enter", Shapes::InputCode::Key::Enter },
            { "Tab", Shapes::InputCode::Key::Tab },
            { "Backspace", Shapes::InputCode::Key::Backspace },
            { "Insert", Shapes::InputCode::Key::Insert },
            { "Delete", Shapes::InputCode::Key::Delete },
            { "Right", Shapes::InputCode::Key::Right },
            { "Left", Shapes::InputCode::Key::Left },
            { "Down", Shapes::InputCode::Key::Down },
            { "Up", Shapes::InputCode::Key::Up },
            { "PageUp", Shapes::InputCode::Key::PageUp },
            { "PageDown", Shapes::InputCode::Key::PageDown },
            { "Home", Shapes::InputCode::Key::Home },
            { "End", Shapes::InputCode::Key::End },
            { "CAPS_LOCK", Shapes::InputCode::Key::CapsLock },
            { "SCROLL_LOCK", Shapes::InputCode::Key::ScrollLock },
            { "NumLock", Shapes::InputCode::Key::NumLock },
            { "PrintScreen", Shapes::InputCode::Key::PrintScreen },
            { "Pasue", Shapes::InputCode::Key::Pause },
            { "LeftShift", Shapes::InputCode::Key::LeftShift },
            { "LeftControl", Shapes::InputCode::Key::LeftControl },
            { "LEFT_ALT", Shapes::InputCode::Key::LeftAlt },
            { "LEFT_SUPER", Shapes::InputCode::Key::LeftSuper },
            { "RightShift", Shapes::InputCode::Key::RightShift },
            { "RightControl", Shapes::InputCode::Key::RightControl },
            { "RIGHT_ALT", Shapes::InputCode::Key::RightAlt },
            { "RIGHT_SUPER", Shapes::InputCode::Key::RightSuper },
            { "Menu", Shapes::InputCode::Key::Menu },
            { "F1", Shapes::InputCode::Key::F1 },
            { "F2", Shapes::InputCode::Key::F2 },
            { "F3", Shapes::InputCode::Key::F3 },
            { "F4", Shapes::InputCode::Key::F4 },
            { "F5", Shapes::InputCode::Key::F5 },
            { "F6", Shapes::InputCode::Key::F6 },
            { "F7", Shapes::InputCode::Key::F7 },
            { "F8", Shapes::InputCode::Key::F8 },
            { "F9", Shapes::InputCode::Key::F9 },
            { "F10", Shapes::InputCode::Key::F10 },
            { "F11", Shapes::InputCode::Key::F11 },
            { "F12", Shapes::InputCode::Key::F12 },
            { "Keypad0", Shapes::InputCode::Key::D0 },
            { "Keypad1", Shapes::InputCode::Key::D1 },
            { "Keypad2", Shapes::InputCode::Key::D2 },
            { "Keypad3", Shapes::InputCode::Key::D3 },
            { "Keypad4", Shapes::InputCode::Key::D4 },
            { "Keypad5", Shapes::InputCode::Key::D5 },
            { "Keypad6", Shapes::InputCode::Key::D6 },
            { "Keypad7", Shapes::InputCode::Key::D7 },
            { "Keypad8", Shapes::InputCode::Key::D8 },
            { "Keypad9", Shapes::InputCode::Key::D9 },
            { "Decimal", Shapes::InputCode::Key::Period },
            { "Divide", Shapes::InputCode::Key::Slash },
            { "Multiply", Shapes::InputCode::Key::KPMultiply },
            { "Subtract", Shapes::InputCode::Key::Minus },
            { "Add", Shapes::InputCode::Key::KPAdd },
            { "KP_EQUAL", Shapes::InputCode::Key::KPEqual }
        };
        state.new_enum<Shapes::InputCode::Key, false>("Key", keyItems); // false makes it read/write in Lua, but its faster

        std::initializer_list<std::pair<sol::string_view, Shapes::InputCode::MouseKey>> mouseItems = {
            { "Left", Shapes::InputCode::MouseKey::ButtonLeft },
            { "Right", Shapes::InputCode::MouseKey::ButtonRight },
            { "Middle", Shapes::InputCode::MouseKey::ButtonMiddle },
        };
        state.new_enum<Shapes::InputCode::MouseKey, false>("MouseButton", mouseItems);
    }

    SharedPtr<Graphics::Texture2D> LoadTexture(const std::string& name, const std::string& path)
    {
        SHAPES_PROFILE_FUNCTION();
        return SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(name, path));
    }

    SharedPtr<Graphics::Texture2D> LoadTextureWithParams(const std::string& name, const std::string& path, Shapes::Graphics::TextureFilter filter, Shapes::Graphics::TextureWrap wrapMode)
    {
        SHAPES_PROFILE_FUNCTION();
        return SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(name, path, Graphics::TextureDesc(filter, filter, wrapMode)));
    }

    void LuaManager::BindECSLua(sol::state& state)
    {
        SHAPES_PROFILE_FUNCTION();

        sol::usertype<entt::registry> enttRegistry = state.new_usertype<entt::registry>("enttRegistry");

        sol::usertype<Entity> entityType               = state.new_usertype<Entity>("Entity", sol::constructors<sol::types<entt::entity, Scene*>>());
        sol::usertype<EntityManager> entityManagerType = state.new_usertype<EntityManager>("EntityManager");
        entityManagerType.set_function("Create", static_cast<Entity (EntityManager::*)()>(&EntityManager::Create));
        entityManagerType.set_function("GetRegistry", &EntityManager::GetRegistry);

        entityType.set_function("Valid", &Entity::Valid);
        entityType.set_function("Destroy", &Entity::Destroy);
        entityType.set_function("SetParent", &Entity::SetParent);
        entityType.set_function("GetParent", &Entity::GetParent);
        entityType.set_function("IsParent", &Entity::IsParent);
        entityType.set_function("GetChildren", &Entity::GetChildren);
        entityType.set_function("SetActive", &Entity::SetActive);
        entityType.set_function("Active", &Entity::Active);

        state.set_function("GetEntityByName", &GetEntityByName);

        state.set_function("AddPyramidEntity", &EntityFactory::AddPyramid);
        state.set_function("AddSphereEntity", &EntityFactory::AddSphere);
        state.set_function("AddLightCubeEntity", &EntityFactory::AddLightCube);

        sol::usertype<NameComponent> nameComponent_type = state.new_usertype<NameComponent>("NameComponent");
        nameComponent_type["name"]                      = &NameComponent::name;
        REGISTER_COMPONENT_WITH_ECS(state, NameComponent, static_cast<NameComponent& (Entity::*)()>(&Entity::AddComponent<NameComponent>));

        sol::usertype<LuaScriptComponent> script_type = state.new_usertype<LuaScriptComponent>("LuaScriptComponent", sol::constructors<sol::types<std::string, Scene*>>());
        REGISTER_COMPONENT_WITH_ECS(state, LuaScriptComponent, static_cast<LuaScriptComponent& (Entity::*)(std::string&&, Scene * &&)>(&Entity::AddComponent<LuaScriptComponent, std::string, Scene*>));
        script_type.set_function("GetCurrentEntity", &LuaScriptComponent::GetCurrentEntity);
        script_type.set_function("SetThisComponent", &LuaScriptComponent::SetThisComponent);

        using namespace Maths;
        REGISTER_COMPONENT_WITH_ECS(state, Transform, static_cast<Transform& (Entity::*)()>(&Entity::AddComponent<Transform>));

        using namespace Graphics;
        sol::usertype<TextComponent> textComponent_type = state.new_usertype<TextComponent>("TextComponent");
        textComponent_type["TextString"]                = &TextComponent::TextString;
        textComponent_type["Colour"]                    = &TextComponent::Colour;
        textComponent_type["MaxWidth"]                  = &TextComponent::MaxWidth;

        REGISTER_COMPONENT_WITH_ECS(state, TextComponent, static_cast<TextComponent& (Entity::*)()>(&Entity::AddComponent<TextComponent>));

        sol::usertype<Sprite> sprite_type = state.new_usertype<Sprite>("Sprite", sol::constructors<sol::types<glm::vec2, glm::vec2, glm::vec4>, Sprite(const SharedPtr<Graphics::Texture2D>&, const glm::vec2&, const glm::vec2&, const glm::vec4&)>());
        sprite_type.set_function("SetTexture", &Sprite::SetTexture);

        REGISTER_COMPONENT_WITH_ECS(state, Sprite, static_cast<Sprite& (Entity::*)(const glm::vec2&, const glm::vec2&, const glm::vec4&)>(&Entity::AddComponent<Sprite, const glm::vec2&, const glm::vec2&, const glm::vec4&>));

        state.new_usertype<Light>(
            "Light",
            "Intensity", &Light::Intensity,
            "Radius", &Light::Radius,
            "Colour", &Light::Colour,
            "Direction", &Light::Direction,
            "Position", &Light::Position,
            "Type", &Light::Type,
            "Angle", &Light::Angle);

        REGISTER_COMPONENT_WITH_ECS(state, Light, static_cast<Light& (Entity::*)()>(&Entity::AddComponent<Light>));

        std::initializer_list<std::pair<sol::string_view, Shapes::Graphics::PrimitiveType>> primitives = {
            { "Cube", Shapes::Graphics::PrimitiveType::Cube },
            { "Plane", Shapes::Graphics::PrimitiveType::Plane },
            { "Quad", Shapes::Graphics::PrimitiveType::Quad },
            { "Pyramid", Shapes::Graphics::PrimitiveType::Pyramid },
            { "Sphere", Shapes::Graphics::PrimitiveType::Sphere },
            { "Capsule", Shapes::Graphics::PrimitiveType::Capsule },
            { "Cylinder", Shapes::Graphics::PrimitiveType::Cylinder },
            { "Terrain", Shapes::Graphics::PrimitiveType::Terrain },
        };

        state.new_enum<Shapes::Graphics::PrimitiveType, false>("PrimitiveType", primitives);

        state.new_usertype<Model>("Model", sol::constructors<Model(const std::string&), Model(Shapes::Graphics::PrimitiveType)>());
        REGISTER_COMPONENT_WITH_ECS(state, Model, static_cast<Model& (Entity::*)(const std::string&)>(&Entity::AddComponent<Model, const std::string&>));

        sol::usertype<Camera> camera_type = state.new_usertype<Camera>("Camera", sol::constructors<Camera(float, float, float, float), Camera(float, float)>());
        camera_type["fov"]                = &Camera::GetFOV;
        camera_type["aspectRatio"]        = &Camera::GetAspectRatio;
        camera_type["nearPlane"]          = &Camera::GetNear;
        camera_type["farPlane"]           = &Camera::GetFar;
        camera_type["SetIsOrthographic"]  = &Camera::SetIsOrthographic;
        camera_type["SetNearPlane"]       = &Camera::SetNear;
        camera_type["SetFarPlane"]        = &Camera::SetFar;

        REGISTER_COMPONENT_WITH_ECS(state, Camera, static_cast<Camera& (Entity::*)(const float&, const float&)>(&Entity::AddComponent<Camera, const float&, const float&>));

        sol::usertype<RigidBody3DComponent> RigidBody3DComponent_type = state.new_usertype<RigidBody3DComponent>("RigidBody3DComponent", sol::constructors<sol::types<const SharedPtr<RigidBody3D>&>>());
        RigidBody3DComponent_type.set_function("GetRigidBody", &RigidBody3DComponent::GetRigidBody);
        REGISTER_COMPONENT_WITH_ECS(state, RigidBody3DComponent, static_cast<RigidBody3DComponent& (Entity::*)(SharedPtr<RigidBody3D>&)>(&Entity::AddComponent<RigidBody3DComponent, SharedPtr<RigidBody3D>&>));

        sol::usertype<RigidBody2DComponent> RigidBody2DComponent_type = state.new_usertype<RigidBody2DComponent>("RigidBody2DComponent", sol::constructors<sol::types<const RigidBodyParameters&>>());
        RigidBody2DComponent_type.set_function("GetRigidBody", &RigidBody2DComponent::GetRigidBody);

        REGISTER_COMPONENT_WITH_ECS(state, RigidBody2DComponent, static_cast<RigidBody2DComponent& (Entity::*)(const RigidBodyParameters&)>(&Entity::AddComponent<RigidBody2DComponent, const RigidBodyParameters&>));

        REGISTER_COMPONENT_WITH_ECS(state, SoundComponent, static_cast<SoundComponent& (Entity::*)()>(&Entity::AddComponent<SoundComponent>));

        // state.set_function("LoadMesh", &ModelLoader::LoadModel);
        // TODO MODEL
        sol::usertype<Graphics::Mesh> mesh_type = state.new_usertype<Graphics::Mesh>("Mesh");

        std::initializer_list<std::pair<sol::string_view, Shapes::Graphics::TextureFilter>> textureFilter = {
            { "None", Shapes::Graphics::TextureFilter::NONE },
            { "Linear", Shapes::Graphics::TextureFilter::LINEAR },
            { "Nearest", Shapes::Graphics::TextureFilter::NEAREST }
        };

        std::initializer_list<std::pair<sol::string_view, Shapes::Graphics::TextureWrap>> textureWrap = {
            { "None", Shapes::Graphics::TextureWrap::NONE },
            { "Repeat", Shapes::Graphics::TextureWrap::REPEAT },
            { "Clamp", Shapes::Graphics::TextureWrap::CLAMP },
            { "MirroredRepeat", Shapes::Graphics::TextureWrap::MIRRORED_REPEAT },
            { "ClampToEdge", Shapes::Graphics::TextureWrap::CLAMP_TO_EDGE },
            { "ClampToBorder", Shapes::Graphics::TextureWrap::CLAMP_TO_BORDER }
        };

        state.set_function("LoadMesh", &CreatePrimative);

        state.new_enum<Shapes::Graphics::TextureWrap, false>("TextureWrap", textureWrap);
        state.new_enum<Shapes::Graphics::TextureFilter, false>("TextureFilter", textureFilter);

        state.set_function("LoadTexture", &LoadTexture);
        state.set_function("LoadTextureWithParams", &LoadTextureWithParams);
    }

    static float LuaRand(float a, float b)
    {
        return Random32::Rand(a, b);
    }

    void LuaManager::BindSceneLua(sol::state& state)
    {
        sol::usertype<Scene> scene_type = state.new_usertype<Scene>("Scene");
        scene_type.set_function("GetRegistry", &Scene::GetRegistry);
        scene_type.set_function("GetEntityManager", &Scene::GetEntityManager);

        sol::usertype<Graphics::Texture2D> texture2D_type = state.new_usertype<Graphics::Texture2D>("Texture2D");
        texture2D_type.set_function("CreateFromFile", &Graphics::Texture2D::CreateFromFile);

        state.set_function("Rand", &LuaRand);
    }

    static void SwitchSceneByIndex(int index)
    {
        Application::Get().GetSceneManager()->SwitchScene(index);
    }

    static void SwitchScene()
    {
        Application::Get().GetSceneManager()->SwitchScene();
    }

    static void SwitchSceneByName(const std::string& name)
    {
        Application::Get().GetSceneManager()->SwitchScene(name);
    }

    static void SetPhysicsDebugFlags(int flags)
    {
        Application::Get().GetSystem<ShapesPhysicsEngine>()->SetDebugDrawFlags(flags);
    }

    void LuaManager::BindAppLua(sol::state& state)
    {
        sol::usertype<Application> app_type = state.new_usertype<Application>("Application");
        state.set_function("SwitchSceneByIndex", &SwitchSceneByIndex);
        state.set_function("SwitchSceneByName", &SwitchSceneByName);
        state.set_function("SwitchScene", &SwitchScene);
        state.set_function("SetPhysicsDebugFlags", &SetPhysicsDebugFlags);

        std::initializer_list<std::pair<sol::string_view, Shapes::PhysicsDebugFlags>> physicsDebugFlags = {
            { "CONSTRAINT", Shapes::PhysicsDebugFlags::CONSTRAINT },
            { "MANIFOLD", Shapes::PhysicsDebugFlags::MANIFOLD },
            { "COLLISIONVOLUMES", Shapes::PhysicsDebugFlags::COLLISIONVOLUMES },
            { "COLLISIONNORMALS", Shapes::PhysicsDebugFlags::COLLISIONNORMALS },
            { "AABB", Shapes::PhysicsDebugFlags::AABB },
            { "LINEARVELOCITY", Shapes::PhysicsDebugFlags::LINEARVELOCITY },
            { "LINEARFORCE", Shapes::PhysicsDebugFlags::LINEARFORCE },
            { "BROADPHASE", Shapes::PhysicsDebugFlags::BROADPHASE },
            { "BROADPHASE_PAIRS", Shapes::PhysicsDebugFlags::BROADPHASE_PAIRS },
            { "BOUNDING_RADIUS", Shapes::PhysicsDebugFlags::BOUNDING_RADIUS },
        };

        state.new_enum<PhysicsDebugFlags, false>("PhysicsDebugFlags", physicsDebugFlags);

        app_type.set_function("GetWindowSize", &Application::GetWindowSize);
        state.set_function("GetAppInstance", &Application::Get);
    }
}
