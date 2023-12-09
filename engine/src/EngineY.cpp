#include "EngineY.hpp"
#include "rendering/renderer.hpp"

namespace EY {
    /// Getters ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    EngineY::Window* Window() {
        return  EngineY::Renderer::Instance()->ActiveWindow();
    }

    EngineY::Renderer* Renderer() {
        return  EngineY::Renderer::Instance();
    }

    EngineY::ResourceHandler* ResourceHandler() {
        return  EngineY::ResourceHandler::Instance();
    }
 
    EngineY::ScriptEngine* ScriptEngine() {
        return  EngineY::ScriptEngine::Instance();
    }
 
    EngineY::EventManager* EventManager() {
        return  EngineY::EventManager::Instance();
    }
 
    EngineY::TaskManager* TaskManager() {
        return  EngineY::TaskManager::Instance();
    }

    EngineY::PhysicsEngine* PhysicsEngine() {
        return  EngineY::PhysicsEngine::Instance();
    }
 
    EngineY::SceneManager* SceneManager() {
        return  EngineY::SceneManager::Instance();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Engine Functions ////////////////////////////////////////////////////////////////////////////////////////////////////
    void DispatchEvent(EngineY::Event* event) {
         EngineY::EventManager::Instance()->DispatchEvent(event);
    }
 
    void ShaderReload() {
         EngineY::ResourceHandler::Instance()->ReloadShaders();
    }
 
    void ScriptReload() {
         EngineY::ScriptEngine::Instance()->ReloadProjectModules();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Event Callback Registration //////////////////////////////////////////////////////////////////////////////////////////// 
    void RegisterWindowResizeCallback(std::function<bool(EngineY::WindowResized*)> callback , const std::string& name) {
         EngineY::EventManager::Instance()->RegisterWindowResizedCallback(callback , name);
    }

    void RegisterWindowMinimizeCallback(std::function<bool(EngineY::WindowMinimized*)> callback , const std::string& name) {
         EngineY::EventManager::Instance()->RegisterWindowMinimizedCallback(callback , name);
    }

    void RegisterWindowCloseCallback(std::function<bool(EngineY::WindowClosed*)> callback , const std::string& name) {
         EngineY::EventManager::Instance()->RegisterWindowClosedCallback(callback , name);
    }

    void RegisterKeyPressCallback(std::function<bool(EngineY::KeyPressed*)> callback , const std::string& name) {
         EngineY::EventManager::Instance()->RegisterKeyPressedCallback(callback , name);
    }

    void RegisterKeyReleaseCallback(std::function<bool(EngineY::KeyReleased*)> callback , const std::string& name) {
         EngineY::EventManager::Instance()->RegisterKeyReleasedCallback(callback , name);
    }

    void RegisterKeyHeldCallback(std::function<bool(EngineY::KeyHeld*)> callback , const std::string& name) {
         EngineY::EventManager::Instance()->RegisterKeyHeldCallback(callback , name);
    }

    void RegisterMouseMoveCallback(std::function<bool(EngineY::MouseMoved*)> callback , const std::string& name) {
         EngineY::EventManager::Instance()->RegisterMouseMovedCallback(callback , name);
    }

    void RegisterMouseScrollCallback(std::function<bool(EngineY::MouseScrolled*)> callback , const std::string& name) {
         EngineY::EventManager::Instance()->RegisterMouseScrolledCallback(callback , name);
    }

    void RegisterMouseButtonPresseCallback(std::function<bool(EngineY::MouseButtonPressed*)> callback , const std::string& name) {
         EngineY::EventManager::Instance()->RegisterMouseButtonPressedCallback(callback , name);
    }

    void RegisterMouseButtonReleaseCallback(std::function<bool(EngineY::MouseButtonReleased*)> callback , const std::string& name) {
         EngineY::EventManager::Instance()->RegisterMouseButtonReleasedCallback(callback , name);
    }

    void RegisterMouseButtonHeldCallback(std::function<bool(EngineY::MouseButtonHeld*)> callback , const std::string& name) {
         EngineY::EventManager::Instance()->RegisterMouseButtonHeldCallback(callback , name);
    }

    void RegisterSceneLoadCallback(std::function<bool(EngineY::SceneLoad*)> callback , const std::string& name) {
         EngineY::EventManager::Instance()->RegisterSceneLoadCallback(callback , name);
    }

    void RegisterSceneStartCallback(std::function<bool(EngineY::SceneStart*)> callback , const std::string& name) {
         EngineY::EventManager::Instance()->RegisterSceneStartCallback(callback , name);
    }

    void RegisterSceneStopCallback(std::function<bool(EngineY::SceneStop*)> callback , const std::string& name) {
         EngineY::EventManager::Instance()->RegisterSceneStopCallback(callback , name);
    }

    void RegisterSceneUnloadCallback(std::function<bool(EngineY::SceneUnload*)> callback , const std::string& name) {
         EngineY::EventManager::Instance()->RegisterSceneUnloadCallback(callback , name);
    }

    void RegisterEditorPlayCallback(std::function<bool(EngineY::EditorPlay*)> callback , const std::string& name) {
         EngineY::EventManager::Instance()->RegisterEditorPlayCallback(callback , name);
    }

    void RegisterEditorPauseCallback(std::function<bool(EngineY::EditorPause*)> callback , const std::string& name) {
         EngineY::EventManager::Instance()->RegisterEditorPauseCallback(callback , name);
    }

    void RegisterEditorStopCallback(std::function<bool(EngineY::EditorStop*)> callback , const std::string& name) {
         EngineY::EventManager::Instance()->RegisterEditorStopCallback(callback , name);
    }  
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
     
    /// Resource Functions ///////////////////////////////////////////////////////////////////////////////////////////////////
    EngineY::Ref<EngineY::Shader> GetCoreShader(const std::string& name) {
        return  EngineY::ResourceHandler::Instance()->GetCoreShader(name);
    }

    EngineY::Ref<EngineY::Shader> GetShader(const std::string& name) {
        return  EngineY::ResourceHandler::Instance()->GetShader(name);
    }

    EngineY::Ref<EngineY::Texture> GetCoreTexture(const std::string& name) {
        return  EngineY::ResourceHandler::Instance()->GetCoreTexture(name);
    }

    EngineY::Ref<EngineY::Texture> GetTexture(const std::string& name) {
        return  EngineY::ResourceHandler::Instance()->GetTexture(name);
    }
 
    EngineY::Ref<EngineY::VertexArray> GetPrimitiveVAO(const std::string& name) {
        return  EngineY::ResourceHandler::Instance()->GetPrimitiveVAO(name);
    }

    EngineY::Model* GetCoreModel(const std::string& name) {
        return  EngineY::ResourceHandler::Instance()->GetCoreModel(name);
    }

    EngineY::Model* GetModel(const std::string& name) {
        return  EngineY::ResourceHandler::Instance()->GetModel(name);
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

namespace EngineY {

    int Main(int argc , char* argv[] , CreateAppFunc create_app) {
        Engine engine;
        engine.CoreInitialize(create_app);
        uint32_t exit_code = engine.Main(argc , argv);
        /*
            handle exit code
        */
        return exit_code;  // result of handling exit code
    }

} // namespace EngineY

#ifdef ENGINEY_WINDOWS              
HINSTANCE YE2_hinstance = nullptr;
int WINAPI WinMain(HINSTANCE hInstance , HINSTANCE hPrevInstance , LPSTR lpCmdLine , int nCmdShow) {     
    YE2_hinstance = hInstance;     
#ifdef YE_DEBUG_BUILD                                                           
    __try {
        return YE2Entry(__argc , __argv);
    } __except(CrashHandlerException(GetExceptionInformation())) {
        return 1;
    }
#else                                                                        
    return YE2Entry(__argc , __argv);
#endif
}
#endif                                      

#ifdef ENGINEY_LINUX
int main(int argc , char* argv[]) {
    /// \todo set up linux stack trace
    return YE2Entry(argc , argv);
}
#endif
