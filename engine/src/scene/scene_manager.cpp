#include "scene/scene_manager.hpp"

#include <filesystem>

#include "log.hpp"
#include "scene/scene.hpp"
#include "parsing/yscript/node_builder.hpp"

namespace YE {

    SceneManager* SceneManager::singleton = nullptr;

    SceneManager* SceneManager::Instance() {
        if (singleton == nullptr) {
            singleton = ynew SceneManager;
        }
        return singleton;
    }
    
    bool SceneManager::LoadProjectFile(const std::string& ys_file) {
        ENTER_FUNCTION_TRACE();

        std::filesystem::path project_path = ys_file;
        if (!std::filesystem::exists(project_path)) {
            ENGINE_ERROR("Could not find project file: {0}" , project_path.string());
            return false;
        } else {
            ENGINE_INFO("Found project file: {0}" , project_path.string());
        }

        YE::YScriptLexer lexer(project_path.string());

        auto [src , tokens] = lexer.Lex();

        YE::YScriptParser parser(tokens);
        ProjectAst parse_tree = parser.Parse();

        YS::Interpreter interpreter(parse_tree);
        YS::ProjectMetadata metadata = interpreter.ProjectMetadata();

        current_scene = interpreter.BuildScene(true);

        EXIT_FUNCTION_TRACE();        
        return true;
    }
    
    void SceneManager::Cleanup() {
        if (current_scene != nullptr) {
            current_scene->Shutdown();
            ydelete current_scene;
        }

        if (singleton != nullptr) {
            delete singleton;
            singleton = nullptr;
        }
    }

} // namespace YE