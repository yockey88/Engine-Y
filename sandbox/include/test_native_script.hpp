#ifndef YE_EDITOR_TEST_NATIVE_SCRIPT_HPP
#define YE_EDITOR_TEST_NATIVE_SCRIPT_HPP

#include "scene/native_script_entity.hpp"

class Foo : public YE::NativeScriptEntity {
    
    void Hello() {}

    void Print(const std::string& str) {}

    public:
        Foo() {}
        virtual ~Foo() {}

        virtual void Create() override {}

        virtual void Update(float dt) override {}

        virtual void Destroy() override {}

};

#endif // !YE_EDITOR_TEST_NATIVE_SCRIPT_HPP