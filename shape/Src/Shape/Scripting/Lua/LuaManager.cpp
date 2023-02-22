#include "hzpch.h"
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
#include "Physics/ShapePhysicsEngine/ShapePhysicsEngine.h"

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
    struct unique_usertype_traits<Shape::SharedPtr<T>>
    {
        typedef T type;
        typedef Shape::SharedPtr<T> actual_type;
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
    struct unique_usertype_traits<Shape::UniquePtr<T>>
    {
        typedef T type;
        typedef Shape::UniquePtr<T> actual_type;
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
    struct unique_usertype_traits<Shape::WeakPtr<T>>
    {
        typedef T type;
        typedef Shape::WeakPtr<T> actual_type;
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

namespace Shape
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
        SHAPE_PROFILE_FUNCTION();

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
        SHAPE_PROFILE_FUNCTION();
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
        SHAPE_PROFILE_FUNCTION();
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
        SHAPE_PROFILE_FUNCTION();
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
        SHAPE_PROFILE_FUNCTION();
        auto log = state.create_table("Log");

        log.set_function("Trace", [&](sol::this_state s, std::string_view message)
                         { SHAPE_LOG_TRACE(message); });

        log.set_function("Info", [&](sol::this_state s, std::string_view message)
                         { SHAPE_LOG_TRACE(message); });

        log.set_function("Warn", [&](sol::this_state s, std::string_view message)
                         { SHAPE_LOG_WARN(message); });

        log.set_function("Error", [&](sol::this_state s, std::string_view message)
                         { SHAPE_LOG_ERROR(message); });

        log.set_function("Critical", [&](sol::this_state s, std::string_view message)
                         { SHAPE_LOG_CRITICAL(message); });
    }

    void LuaManager::BindInputLua(sol::state& state)
    {
        SHAPE_PROFILE_FUNCTION();
        auto input = state["Input"].get_or_create<sol::table>();

        input.set_function("GetKeyPressed", [](Shape::InputCode::Key key) -> bool
                           { return Input::Get().GetKeyPressed(key); });

        input.set_function("GetKeyHeld", [](Shape::InputCode::Key key) -> bool
                           { return Input::Get().GetKeyHeld(key); });

        input.set_function("GetMouseClicked", [](Shape::InputCode::MouseKey key) -> bool
                           { return Input::Get().GetMouseClicked(key); });

        input.set_function("GetMouseHeld", [](Shape::InputCode::MouseKey key) -> bool
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

        std::initializer_list<std::pair<sol::string_view, Shape::InputCode::Key>> keyItems = {
            { "A", Shape::InputCode::Key::A },
            { "B", Shape::InputCode::Key::B },
            { "C", Shape::InputCode::Key::C },
            { "D", Shape::InputCode::Key::D },
            { "E", Shape::InputCode::Key::E },
            { "F", Shape::InputCode::Key::F },
            { "H", Shape::InputCode::Key::G },
            { "G", Shape::InputCode::Key::H },
            { "I", Shape::InputCode::Key::I },
            { "J", Shape::InputCode::Key::J },
            { "K", Shape::InputCode::Key::K },
            { "L", Shape::InputCode::Key::L },
            { "M", Shape::InputCode::Key::M },
            { "N", Shape::InputCode::Key::N },
            { "O", Shape::InputCode::Key::O },
            { "P", Shape::InputCode::Key::P },
            { "Q", Shape::InputCode::Key::Q },
            { "R", Shape::InputCode::Key::R },
            { "S", Shape::InputCode::Key::S },
            { "T", Shape::InputCode::Key::T },
            { "U", Shape::InputCode::Key::U },
            { "V", Shape::InputCode::Key::V },
            { "W", Shape::InputCode::Key::W },
            { "X", Shape::InputCode::Key::X },
            { "Y", Shape::InputCode::Key::Y },
            { "Z", Shape::InputCode::Key::Z },
            //{ "UNKOWN", Shape::InputCode::Key::Unknown },
            { "Space", Shape::InputCode::Key::Space },
            { "Escape", Shape::InputCode::Key::Escape },
            { "APOSTROPHE", Shape::InputCode::Key::Apostrophe },
            { "Comma", Shape::InputCode::Key::Comma },
            { "MINUS", Shape::InputCode::Key::Minus },
            { "PERIOD", Shape::InputCode::Key::Period },
            { "SLASH", Shape::InputCode::Key::Slash },
            { "SEMICOLON", Shape::InputCode::Key::Semicolon },
            { "EQUAL", Shape::InputCode::Key::Equal },
            { "LEFT_BRACKET", Shape::InputCode::Key::LeftBracket },
            { "BACKSLASH", Shape::InputCode::Key::Backslash },
            { "RIGHT_BRACKET", Shape::InputCode::Key::RightBracket },
            //{ "BACK_TICK", Shape::InputCode::Key::BackTick },
            { "Enter", Shape::InputCode::Key::Enter },
            { "Tab", Shape::InputCode::Key::Tab },
            { "Backspace", Shape::InputCode::Key::Backspace },
            { "Insert", Shape::InputCode::Key::Insert },
            { "Delete", Shape::InputCode::Key::Delete },
            { "Right", Shape::InputCode::Key::Right },
            { "Left", Shape::InputCode::Key::Left },
            { "Down", Shape::InputCode::Key::Down },
            { "Up", Shape::InputCode::Key::Up },
            { "PageUp", Shape::InputCode::Key::PageUp },
            { "PageDown", Shape::InputCode::Key::PageDown },
            { "Home", Shape::InputCode::Key::Home },
            { "End", Shape::InputCode::Key::End },
            { "CAPS_LOCK", Shape::InputCode::Key::CapsLock },
            { "SCROLL_LOCK", Shape::InputCode::Key::ScrollLock },
            { "NumLock", Shape::InputCode::Key::NumLock },
            { "PrintScreen", Shape::InputCode::Key::PrintScreen },
            { "Pasue", Shape::InputCode::Key::Pause },
            { "LeftShift", Shape::InputCode::Key::LeftShift },
            { "LeftControl", Shape::InputCode::Key::LeftControl },
            { "LEFT_ALT", Shape::InputCode::Key::LeftAlt },
            { "LEFT_SUPER", Shape::InputCode::Key::LeftSuper },
            { "RightShift", Shape::InputCode::Key::RightShift },
            { "RightControl", Shape::InputCode::Key::RightControl },
            { "RIGHT_ALT", Shape::InputCode::Key::RightAlt },
            { "RIGHT_SUPER", Shape::InputCode::Key::RightSuper },
            { "Menu", Shape::InputCode::Key::Menu },
            { "F1", Shape::InputCode::Key::F1 },
            { "F2", Shape::InputCode::Key::F2 },
            { "F3", Shape::InputCode::Key::F3 },
            { "F4", Shape::InputCode::Key::F4 },
            { "F5", Shape::InputCode::Key::F5 },
            { "F6", Shape::InputCode::Key::F6 },
            { "F7", Shape::InputCode::Key::F7 },
            { "F8", Shape::InputCode::Key::F8 },
            { "F9", Shape::InputCode::Key::F9 },
            { "F10", Shape::InputCode::Key::F10 },
            { "F11", Shape::InputCode::Key::F11 },
            { "F12", Shape::InputCode::Key::F12 },
            { "Keypad0", Shape::InputCode::Key::D0 },
            { "Keypad1", Shape::InputCode::Key::D1 },
            { "Keypad2", Shape::InputCode::Key::D2 },
            { "Keypad3", Shape::InputCode::Key::D3 },
            { "Keypad4", Shape::InputCode::Key::D4 },
            { "Keypad5", Shape::InputCode::Key::D5 },
            { "Keypad6", Shape::InputCode::Key::D6 },
            { "Keypad7", Shape::InputCode::Key::D7 },
            { "Keypad8", Shape::InputCode::Key::D8 },
            { "Keypad9", Shape::InputCode::Key::D9 },
            { "Decimal", Shape::InputCode::Key::Period },
            { "Divide", Shape::InputCode::Key::Slash },
            { "Multiply", Shape::InputCode::Key::KPMultiply },
            { "Subtract", Shape::InputCode::Key::Minus },
            { "Add", Shape::InputCode::Key::KPAdd },
            { "KP_EQUAL", Shape::InputCode::Key::KPEqual }
        };
        state.new_enum<Shape::InputCode::Key, false>("Key", keyItems); // false makes it read/write in Lua, but its faster

        std::initializer_list<std::pair<sol::string_view, Shape::InputCode::MouseKey>> mouseItems = {
            { "Left", Shape::InputCode::MouseKey::ButtonLeft },
            { "Right", Shape::InputCode::MouseKey::ButtonRight },
            { "Middle", Shape::InputCode::MouseKey::ButtonMiddle },
        };
        state.new_enum<Shape::InputCode::MouseKey, false>("MouseButton", mouseItems);
    }

    SharedPtr<Graphics::Texture2D> LoadTexture(const std::string& name, const std::string& path)
    {
        SHAPE_PROFILE_FUNCTION();
        return SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(name, path));
    }

    SharedPtr<Graphics::Texture2D> LoadTextureWithParams(const std::string& name, const std::string& path, Shape::Graphics::TextureFilter filter, Shape::Graphics::TextureWrap wrapMode)
    {
        SHAPE_PROFILE_FUNCTION();
        return SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile(name, path, Graphics::TextureDesc(filter, filter, wrapMode)));
    }

    void LuaManager::BindECSLua(sol::state& state)
    {
        SHAPE_PROFILE_FUNCTION();

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

        std::initializer_list<std::pair<sol::string_view, Shape::Graphics::PrimitiveType>> primitives = {
            { "Cube", Shape::Graphics::PrimitiveType::Cube },
            { "Plane", Shape::Graphics::PrimitiveType::Plane },
            { "Quad", Shape::Graphics::PrimitiveType::Quad },
            { "Pyramid", Shape::Graphics::PrimitiveType::Pyramid },
            { "Sphere", Shape::Graphics::PrimitiveType::Sphere },
            { "Capsule", Shape::Graphics::PrimitiveType::Capsule },
            { "Cylinder", Shape::Graphics::PrimitiveType::Cylinder },
            { "Terrain", Shape::Graphics::PrimitiveType::Terrain },
        };

        state.new_enum<Shape::Graphics::PrimitiveType, false>("PrimitiveType", primitives);

        state.new_usertype<Model>("Model", sol::constructors<Model(const std::string&), Model(Shape::Graphics::PrimitiveType)>());
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

        std::initializer_list<std::pair<sol::string_view, Shape::Graphics::TextureFilter>> textureFilter = {
            { "None", Shape::Graphics::TextureFilter::NONE },
            { "Linear", Shape::Graphics::TextureFilter::LINEAR },
            { "Nearest", Shape::Graphics::TextureFilter::NEAREST }
        };

        std::initializer_list<std::pair<sol::string_view, Shape::Graphics::TextureWrap>> textureWrap = {
            { "None", Shape::Graphics::TextureWrap::NONE },
            { "Repeat", Shape::Graphics::TextureWrap::REPEAT },
            { "Clamp", Shape::Graphics::TextureWrap::CLAMP },
            { "MirroredRepeat", Shape::Graphics::TextureWrap::MIRRORED_REPEAT },
            { "ClampToEdge", Shape::Graphics::TextureWrap::CLAMP_TO_EDGE },
            { "ClampToBorder", Shape::Graphics::TextureWrap::CLAMP_TO_BORDER }
        };

        state.set_function("LoadMesh", &CreatePrimative);

        state.new_enum<Shape::Graphics::TextureWrap, false>("TextureWrap", textureWrap);
        state.new_enum<Shape::Graphics::TextureFilter, false>("TextureFilter", textureFilter);

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
        Application::Get().GetSystem<ShapePhysicsEngine>()->SetDebugDrawFlags(flags);
    }

    void LuaManager::BindAppLua(sol::state& state)
    {
        sol::usertype<Application> app_type = state.new_usertype<Application>("Application");
        state.set_function("SwitchSceneByIndex", &SwitchSceneByIndex);
        state.set_function("SwitchSceneByName", &SwitchSceneByName);
        state.set_function("SwitchScene", &SwitchScene);
        state.set_function("SetPhysicsDebugFlags", &SetPhysicsDebugFlags);

        std::initializer_list<std::pair<sol::string_view, Shape::PhysicsDebugFlags>> physicsDebugFlags = {
            { "CONSTRAINT", Shape::PhysicsDebugFlags::CONSTRAINT },
            { "MANIFOLD", Shape::PhysicsDebugFlags::MANIFOLD },
            { "COLLISIONVOLUMES", Shape::PhysicsDebugFlags::COLLISIONVOLUMES },
            { "COLLISIONNORMALS", Shape::PhysicsDebugFlags::COLLISIONNORMALS },
            { "AABB", Shape::PhysicsDebugFlags::AABB },
            { "LINEARVELOCITY", Shape::PhysicsDebugFlags::LINEARVELOCITY },
            { "LINEARFORCE", Shape::PhysicsDebugFlags::LINEARFORCE },
            { "BROADPHASE", Shape::PhysicsDebugFlags::BROADPHASE },
            { "BROADPHASE_PAIRS", Shape::PhysicsDebugFlags::BROADPHASE_PAIRS },
            { "BOUNDING_RADIUS", Shape::PhysicsDebugFlags::BOUNDING_RADIUS },
        };

        state.new_enum<PhysicsDebugFlags, false>("PhysicsDebugFlags", physicsDebugFlags);

        app_type.set_function("GetWindowSize", &Application::GetWindowSize);
        state.set_function("GetAppInstance", &Application::Get);
    }
}
