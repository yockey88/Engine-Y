#include "EngineY.hpp"

class editor_dev : public YE::App {
    public:
        editor_dev() 
            : YE::App("editor_dev") {}
        virtual ~editor_dev() override {}
        
        virtual bool Initialize() override { return true; }
};

DECLARE_APP(editor_dev);