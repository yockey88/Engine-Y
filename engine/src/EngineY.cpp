#include "EngineY.hpp"

#include <queue>
#include <thread>

namespace EngineY {
    /// Getters ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // YE::Engine* Engine() {
    //     return YE::Engine::Instance();
    // }

    YE::CmndLineHandler& CmndLineHandler() {
        return YE::Engine::Instance()->CmndLineHandler();
    }

    YE::Window* Window() {
        return YE::Renderer::Instance()->ActiveWindow();
    }

    YE::Renderer* Renderer() {
        return YE::Renderer::Instance();
    }

    YE::ResourceHandler* ResourceHandler() {
        return YE::ResourceHandler::Instance();
    }
    
    YE::ScriptEngine* ScriptEngine() {
        return YE::ScriptEngine::Instance();
    }
    
    YE::EventManager* EventManager() {
        return YE::EventManager::Instance();
    }
    
    YE::TaskManager* TaskManager() {
        return YE::TaskManager::Instance();
    }

    YE::PhysicsEngine* PhysicsEngine() {
        return YE::PhysicsEngine::Instance();
    }
    
    YE::SceneManager* SceneManager() {
        return YE::SceneManager::Instance();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Engine Functions ////////////////////////////////////////////////////////////////////////////////////////////////////
    void DispatchEvent(YE::Event* event) {
        YE::EventManager::Instance()->DispatchEvent(event);
    }
    
    void ShaderReload() {
        YE::ResourceHandler::Instance()->ReloadShaders();
    }
    
    void ScriptReload() {
        YE::ScriptEngine::Instance()->ReloadProjectModules();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Event Callback Registration //////////////////////////////////////////////////////////////////////////////////////////// 
    void RegisterWindowResizeCallback(std::function<bool(YE::WindowResized*)> callback , const std::string& name) {
        YE::EventManager::Instance()->RegisterWindowResizedCallback(callback , name);
    }

    void RegisterWindowMinimizeCallback(std::function<bool(YE::WindowMinimized*)> callback , const std::string& name) {
        YE::EventManager::Instance()->RegisterWindowMinimizedCallback(callback , name);
    }

    void RegisterWindowCloseCallback(std::function<bool(YE::WindowClosed*)> callback , const std::string& name) {
        YE::EventManager::Instance()->RegisterWindowClosedCallback(callback , name);
    }

    void RegisterKeyPressCallback(std::function<bool(YE::KeyPressed*)> callback , const std::string& name) {
        YE::EventManager::Instance()->RegisterKeyPressedCallback(callback , name);
    }

    void RegisterKeyReleaseCallback(std::function<bool(YE::KeyReleased*)> callback , const std::string& name) {
        YE::EventManager::Instance()->RegisterKeyReleasedCallback(callback , name);
    }

    void RegisterKeyHeldCallback(std::function<bool(YE::KeyHeld*)> callback , const std::string& name) {
        YE::EventManager::Instance()->RegisterKeyHeldCallback(callback , name);
    }

    void RegisterMouseMoveCallback(std::function<bool(YE::MouseMoved*)> callback , const std::string& name) {
        YE::EventManager::Instance()->RegisterMouseMovedCallback(callback , name);
    }

    void RegisterMouseScrollCallback(std::function<bool(YE::MouseScrolled*)> callback , const std::string& name) {
        YE::EventManager::Instance()->RegisterMouseScrolledCallback(callback , name);
    }

    void RegisterMouseButtonPresseCallback(std::function<bool(YE::MouseButtonPressed*)> callback , const std::string& name) {
        YE::EventManager::Instance()->RegisterMouseButtonPressedCallback(callback , name);
    }

    void RegisterMouseButtonReleaseCallback(std::function<bool(YE::MouseButtonReleased*)> callback , const std::string& name) {
        YE::EventManager::Instance()->RegisterMouseButtonReleasedCallback(callback , name);
    }

    void RegisterMouseButtonHeldCallback(std::function<bool(YE::MouseButtonHeld*)> callback , const std::string& name) {
        YE::EventManager::Instance()->RegisterMouseButtonHeldCallback(callback , name);
    }

    void RegisterSceneLoadCallback(std::function<bool(YE::SceneLoad*)> callback , const std::string& name) {
        YE::EventManager::Instance()->RegisterSceneLoadCallback(callback , name);
    }

    void RegisterSceneStartCallback(std::function<bool(YE::SceneStart*)> callback , const std::string& name) {
        YE::EventManager::Instance()->RegisterSceneStartCallback(callback , name);
    }

    void RegisterSceneStopCallback(std::function<bool(YE::SceneStop*)> callback , const std::string& name) {
        YE::EventManager::Instance()->RegisterSceneStopCallback(callback , name);
    }

    void RegisterSceneUnloadCallback(std::function<bool(YE::SceneUnload*)> callback , const std::string& name) {
        YE::EventManager::Instance()->RegisterSceneUnloadCallback(callback , name);
    }

    void RegisterEditorPlayCallback(std::function<bool(YE::EditorPlay*)> callback , const std::string& name) {
        YE::EventManager::Instance()->RegisterEditorPlayCallback(callback , name);
    }

    void RegisterEditorPauseCallback(std::function<bool(YE::EditorPause*)> callback , const std::string& name) {
        YE::EventManager::Instance()->RegisterEditorPauseCallback(callback , name);
    }

    void RegisterEditorStopCallback(std::function<bool(YE::EditorStop*)> callback , const std::string& name) {
        YE::EventManager::Instance()->RegisterEditorStopCallback(callback , name);
    }  
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  

    /// Render Callback Registration //////////////////////////////////////////////////////////////////////////////////////////
    void RegisterPreRenderCallback(std::function<void()> callback , const std::string& name) {
        YE::Renderer::Instance()->RegisterPreRenderCallback(callback , name);
    }

    void RegisterPostRenderCallback(std::function<void()> callback , const std::string& name) {
        YE::Renderer::Instance()->RegisterPostRenderCallback(callback , name);
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
    /// Framebuffer Functions ////////////////////////////////////////////////////////////////////////////////////////////////
    void PushFramebuffer(const std::string& name , YE::Framebuffer* framebuffer) {
        YE::Renderer::Instance()->PushFramebuffer(name , framebuffer);
    }

    void ActivateFramebuffer(const std::string& name) {
        YE::Renderer::Instance()->ActivateFramebuffer(name);
    }

    void ActivateLastFramebuffer() {
        YE::Renderer::Instance()->ActivateLastFramebuffer();
    }

    void SetDefaultFramebuffer(const std::string& name) {
        YE::Renderer::Instance()->SetDefaultFramebuffer(name);
    }

    void RevertToDefaultFramebuffer() {
        YE::Renderer::Instance()->RevertToDefaultFramebuffer();
    }

    void DeactivateFramebuffer() {
        YE::Renderer::Instance()->DeactivateFramebuffer();
    }

    void PopFramebuffer(const std::string& name) {
        YE::Renderer::Instance()->PopFramebuffer(name);
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Resource Functions ///////////////////////////////////////////////////////////////////////////////////////////////////
    YE::Shader* GetCoreShader(const std::string& name) {
        return YE::ResourceHandler::Instance()->GetCoreShader(name);
    }

    YE::Shader* GetShader(const std::string& name) {
        return YE::ResourceHandler::Instance()->GetShader(name);
    }

    YE::Texture* GetCoreTexture(const std::string& name) {
        return YE::ResourceHandler::Instance()->GetCoreTexture(name);
    }

    YE::Texture* GetTexture(const std::string& name) {
        return YE::ResourceHandler::Instance()->GetTexture(name);
    }
    
    YE::VertexArray* GetPrimitiveVAO(const std::string& name) {
        return YE::ResourceHandler::Instance()->GetPrimitiveVAO(name);
    }

    YE::Model* GetCoreModel(const std::string& name) {
        return YE::ResourceHandler::Instance()->GetCoreModel(name);
    }

    YE::Model* GetModel(const std::string& name) {
        return YE::ResourceHandler::Instance()->GetModel(name);
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#ifdef YE_PLATFORM_WIN              
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

#ifdef YE_PLATFORM_LINUX
int main(int argc , char* argv[]) {
    /// \todo set up linux stack trace
    return YE2Entry(argc , argv);
}
#endif