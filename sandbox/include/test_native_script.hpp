#ifndef YE_EDITOR_TEST_NATIVE_SCRIPT_HPP
#define YE_EDITOR_TEST_NATIVE_SCRIPT_HPP

#include "scene/native_script_entity.hpp"

class Foo : public YE::NativeScriptEntity {
    
    void Hello() {
        YE_TRACE("Foo says hello from C++!");
    }

    void Print(const std::string& str) {
        YE_TRACE(str);
    }

    public:
        Foo() {}
        virtual ~Foo() {
            YE_TRACE("Foo::~Foo()");
        }

        virtual void Create() override {
            YE_TRACE("Foo::Create()");
            Hello();
        }

        virtual void Update(float dt) override {
            YE_TRACE("Foo::Update()");
        }

        virtual void Destroy() override {
            YE_TRACE("Foo::Destroy()");
        }

};

#endif // !YE_EDITOR_TEST_NATIVE_SCRIPT_HPP