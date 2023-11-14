#include "parsing/yscript/yscript_lexer.hpp"

#include "log.hpp"
#include "core/filesystem.hpp"

namespace YE {

    void YScriptLexer::InitKeywordMaps() {
        keyword_map[Hash::FNV32("project")] = YScriptTokenType::PROJECT;
        keyword_map[Hash::FNV32("name")] = YScriptTokenType::NAME;
        keyword_map[Hash::FNV32("author")] = YScriptTokenType::AUTHOR;
        keyword_map[Hash::FNV32("version")] = YScriptTokenType::VERSION;
        keyword_map[Hash::FNV32("description")] = YScriptTokenType::DESCRIPTION;
        keyword_map[Hash::FNV32("resources")] = YScriptTokenType::RESOURCES;
        keyword_map[Hash::FNV32("path")] = YScriptTokenType::PATH;
        keyword_map[Hash::FNV32("window")] = YScriptTokenType::WINDOW; 
        keyword_map[Hash::FNV32("title")] = YScriptTokenType::TITLE;
        keyword_map[Hash::FNV32("min_scale")] = YScriptTokenType::MIN_SCALE;
        keyword_map[Hash::FNV32("clear_color")] = YScriptTokenType::CLEAR_COLOR;
        keyword_map[Hash::FNV32("flags")] = YScriptTokenType::FLAGS;
        keyword_map[Hash::FNV32("color_bits")] = YScriptTokenType::COLOR_BITS;
        keyword_map[Hash::FNV32("stencil_size")] = YScriptTokenType::STENCIL_SIZE;
        keyword_map[Hash::FNV32("multisample_buffers")] = YScriptTokenType::MULTISAMPLE_BUFFERS;
        keyword_map[Hash::FNV32("multisample_samples")] = YScriptTokenType::MULTISAMPLE_SAMPLES;
        keyword_map[Hash::FNV32("fullscreen")] = YScriptTokenType::FULLSCREEN;
        keyword_map[Hash::FNV32("vsync")] = YScriptTokenType::VSYNC;
        keyword_map[Hash::FNV32("rendering_to_screen")] = YScriptTokenType::RENDERING_TO_SCREEN;
        keyword_map[Hash::FNV32("accelerated_visual")] = YScriptTokenType::ACCELERATED_VISUAL;
        keyword_map[Hash::FNV32("node")] = YScriptTokenType::NODE;
        keyword_map[Hash::FNV32("scene")] = YScriptTokenType::SCENE;
        keyword_map[Hash::FNV32("entity")] = YScriptTokenType::ENTITY;
        keyword_map[Hash::FNV32("transform")] = YScriptTokenType::TRANSFORM;
        keyword_map[Hash::FNV32("position")] = YScriptTokenType::POSITION;
        keyword_map[Hash::FNV32("rotation")] = YScriptTokenType::ROTATION;
        keyword_map[Hash::FNV32("scale")] = YScriptTokenType::SCALE;
        keyword_map[Hash::FNV32("renderable")] = YScriptTokenType::RENDERABLE;
        keyword_map[Hash::FNV32("textured_renderable")] = YScriptTokenType::TEXTURED_RENDERABLE;
        keyword_map[Hash::FNV32("renderable_model")] = YScriptTokenType::RENDERABLE_MODEL;
        keyword_map[Hash::FNV32("mesh")] = YScriptTokenType::MESH;
        keyword_map[Hash::FNV32("texture")] = YScriptTokenType::TEXTURE;
        keyword_map[Hash::FNV32("shader")] = YScriptTokenType::SHADER;
        keyword_map[Hash::FNV32("model")] = YScriptTokenType::MODEL;
        keyword_map[Hash::FNV32("point_light")] = YScriptTokenType::POINT_LIGHT;
        keyword_map[Hash::FNV32("color")] = YScriptTokenType::COLOR;
        keyword_map[Hash::FNV32("ambient")] = YScriptTokenType::AMBIENT;
        keyword_map[Hash::FNV32("diffuse")] = YScriptTokenType::DIFFUSE;
        keyword_map[Hash::FNV32("specular")] = YScriptTokenType::SPECULAR;
        keyword_map[Hash::FNV32("constant")] = YScriptTokenType::CONSTANT;
        keyword_map[Hash::FNV32("linear")] = YScriptTokenType::LINEAR;
        keyword_map[Hash::FNV32("quadratic")] = YScriptTokenType::QUADRATIC;
        keyword_map[Hash::FNV32("camera")] = YScriptTokenType::CAMERA;
        keyword_map[Hash::FNV32("orthographic")] = YScriptTokenType::ORTHOGRAPHIC;
        keyword_map[Hash::FNV32("perspective")] = YScriptTokenType::PERSPECTIVE;
        keyword_map[Hash::FNV32("front")] = YScriptTokenType::FRONT;
        keyword_map[Hash::FNV32("up")] = YScriptTokenType::UP;
        keyword_map[Hash::FNV32("right")] = YScriptTokenType::RIGHT;
        keyword_map[Hash::FNV32("world_up")] = YScriptTokenType::WORLD_UP;
        keyword_map[Hash::FNV32("euler_angles")] = YScriptTokenType::EULER_ANGLES;
        keyword_map[Hash::FNV32("viewport")] = YScriptTokenType::VIEWPORT;
        keyword_map[Hash::FNV32("clip")] = YScriptTokenType::CLIP;
        keyword_map[Hash::FNV32("speed")] = YScriptTokenType::SPEED;
        keyword_map[Hash::FNV32("sensitivity")] = YScriptTokenType::SENSITIVITY;
        keyword_map[Hash::FNV32("fov")] = YScriptTokenType::FOV;
        keyword_map[Hash::FNV32("zoom")] = YScriptTokenType::ZOOM;
        keyword_map[Hash::FNV32("script")] = YScriptTokenType::SCRIPT;
        keyword_map[Hash::FNV32("native_script")] = YScriptTokenType::NATIVE_SCRIPT;
        keyword_map[Hash::FNV32("object")] = YScriptTokenType::OBJECT;
        keyword_map[Hash::FNV32("physics_body")] = YScriptTokenType::PHYSICS_BODY;
        keyword_map[Hash::FNV32("body_type")] = YScriptTokenType::PHYSICS_BODY_TYPE;
        keyword_map[Hash::FNV32("box_collider")] = YScriptTokenType::BOX_COLLIDER;
        keyword_map[Hash::FNV32("sphere_collider")] = YScriptTokenType::SPHERE_COLLIDER;
        keyword_map[Hash::FNV32("capsule_collider")] = YScriptTokenType::CAPSULE_COLLIDER;
        keyword_map[Hash::FNV32("model_collider")] = YScriptTokenType::MODEL_COLLIDER;
        keyword_map[Hash::FNV32("radius")] = YScriptTokenType::RADIUS;
        keyword_map[Hash::FNV32("height")] = YScriptTokenType::HEIGHT;
        keyword_map[Hash::FNV32("true")] = YScriptTokenType::TRUE_TKN;
        keyword_map[Hash::FNV32("false")] = YScriptTokenType::FALSE_TKN;
        keyword_map[Hash::FNV32("fn")] = YScriptTokenType::FN;
        keyword_map[Hash::FNV32("main")] = YScriptTokenType::MAIN_FN;
    }

    void YScriptLexer::HandleWhitespace() {
        while (IsWhitespace(Peek())) {
            if (Peek() == '\n') {
                NewLine();
            } else {
                Consume();
            }

            if (IsEOF()) break;
        }
    }

    void YScriptLexer::HandleOperator() {
        char c = Peek();
        if (c == '"' || c == '\'')
            Consume();
        else
            Advance();

        switch (c) {
            case '<':
                if (Check('=')) {
                    Advance();
                    AddToken(YScriptTokenType::LESS_THAN_EQUALS);
                } else {
                    AddToken(YScriptTokenType::LESS_THAN);
                } 
            break;
            case '>':
                if (Check('=')) {
                    Advance();
                    AddToken(YScriptTokenType::GREATER_THAN_EQUALS);
                } else {
                    AddToken(YScriptTokenType::GREATER_THAN);
                }
            break;
            case '(': AddToken(YScriptTokenType::OPEN_PAREN); break;
            case ')': AddToken(YScriptTokenType::CLOSE_PAREN); break;
            case '{': AddToken(YScriptTokenType::OPEN_BRACE); break;
            case '}': AddToken(YScriptTokenType::CLOSE_BRACE); break;
            case '[': AddToken(YScriptTokenType::OPEN_BRACKET); break;
            case ']': AddToken(YScriptTokenType::CLOSE_BRACKET); break;
            case ',': AddToken(YScriptTokenType::COMMA); break;
            case ':':
                if (Check(':')) {
                    Advance();
                    AddToken(YScriptTokenType::COLON_COLON);
                } else {
                    AddToken(YScriptTokenType::COLON); 
                }
            break;
            case '.': AddToken(YScriptTokenType::PERIOD); break;
            case '\'': HandleCharacter(); break;
            case '"':  HandleString(); break;
            case '\\': AddToken(YScriptTokenType::BACKWARD_SLASH); break;
            case '=': AddToken(YScriptTokenType::EQUALS); break;
            case ';': AddToken(YScriptTokenType::SEMICOLON); break;
            case '-':
                if (Check('-')) {
                    Advance();
                    AddToken(YScriptTokenType::MINUS_MINUS);
                } else if (Check('=')) {
                    Advance();
                    AddToken(YScriptTokenType::MINUS_EQUALS);
                } else {
                    AddToken(YScriptTokenType::MINUS);
                }
            break;
            case '+': 
                if (Check('+')) {
                    Advance();
                    AddToken(YScriptTokenType::PLUS_PLUS);
                } else if (Check('=')) {
                    Advance();
                    AddToken(YScriptTokenType::PLUS_EQUALS);
                } else {
                    AddToken(YScriptTokenType::PLUS);
                }
            break;
            case '*': 
                if (Check('=')) {
                    Advance();
                    AddToken(YScriptTokenType::ASTERISK_EQUALS);
                } else {    
                    AddToken(YScriptTokenType::ASTERISK);
                }
            break;
            case '/':
                if (Check('/') || Check('*')) {
                    HandleComment();
                } else if(Check('=')) {
                    Advance();
                    AddToken(YScriptTokenType::FORWARD_SLASH_EQUALS);
                } else {
                    AddToken(YScriptTokenType::FORWARD_SLASH);
                }
            break;
            case '%':
                if (Check('=')) {
                    Advance();
                    AddToken(YScriptTokenType::PERCENT_EQUALS);
                } else {
                    AddToken(YScriptTokenType::PERCENT);
                }
            break;
            case '!':
                if (Check('=')) {
                    Advance();
                    AddToken(YScriptTokenType::EXCLAMATION_EQUALS);
                } else {
                    AddToken(YScriptTokenType::EXCLAMATION);
                }
            break;
            case '?': AddToken(YScriptTokenType::QUESTION); break;
            case '&': 
                if (Check('&')) {
                    Advance();
                    AddToken(YScriptTokenType::LOGICAL_AND);
                } else {
                    throw yscript_lexer_error("Bitwise operators not defined in YScript" , line , column);
                }
            break;
            case '|': 
                if (Check('|')) {
                    Advance();
                    AddToken(YScriptTokenType::LOGICAL_OR);
                } else {
                    throw yscript_lexer_error("Bitwise operators not defined in YScript" , line , column);
                }
            break;
            case '#': AddToken(YScriptTokenType::HASH); break;
            case '$':
                if (Check('<') && CheckNext('>')) {
                    Advance();
                    Advance();
                    AddToken(YScriptTokenType::EMPTY_VALUE);
                } else {
                    AddToken(YScriptTokenType::DOLLAR);
                }
            break;
            case '_': AddToken(YScriptTokenType::UNDERSCORE); break;

            /// \todo maybe add stuff to this language later
            // case '~': AddToken(GlslTokenType::BIT_NOT); break;
            default:
                throw yscript_lexer_error("Undefined Behavior In YScript Lexer" , line , column);
        }
    }

    void YScriptLexer::HandleComment() {
        if (Peek() == '/') {
            while (Peek() != '\n' && !IsEOF())
                Advance();
        } else if (Peek() == '*') {
            Advance();
            while (!IsEOF()) {
                if (Peek() == '\n') {
                    Advance();
                    NewLine(false);
                    continue;
                }
                if (Peek() == '*' && PeekNext() == '/') break;
                Advance();
            }
            
            if (IsEOF())
                throw yscript_lexer_error("Unterminated comment" , line , column);
            
            Advance();
            Advance();
        }

        YScriptToken comment_tkn(YScriptTokenType::COMMENT , line , column , curr_token);
        DiscardToken();
        comments.push_back(comment_tkn);
    }

    void YScriptLexer::HandleNumeric() {
        while (IsNumeric(Peek())) {
            Advance();
            if (IsEOF()) break;
        }

        if (Peek() == '.' && IsNumeric(PeekNext())) {
            Advance();
            while (IsNumeric(Peek())) {
                Advance();
                if (IsEOF()) break;
            }
        }
        
        AddToken(YScriptTokenType::FLOAT);
    }

    void YScriptLexer::HandleAlphaNumeric() {
        while (IsAlphaNumeric(Peek()) || Peek() == '-') { // handles names like "my-node"
            Advance();
            if (IsEOF()) break;
        }

        if (IsKeyword(curr_token)) {
            AddToken(keyword_map[Hash::FNV32(curr_token)]);
        } else {
            AddToken(YScriptTokenType::IDENTIFIER);
        }
    }

    void YScriptLexer::HandleCharacter() {
        Advance();
        
        if (Peek() != '\'')
            throw yscript_lexer_error("Invalid character" , line , column);

        Consume();
        AddToken(YScriptTokenType::CHARACTER);
    }

    void YScriptLexer::HandleString() {
        while (Peek() != '"' && !IsEOF()) {
            Advance();
            if (Peek() == '\n') NewLine(false);
        }

        if (IsEOF())
            throw yscript_lexer_error("Unterminated string" , line , column);

        Consume();
        AddToken(YScriptTokenType::STRING);
    }

    std::pair<std::string , std::vector<YScriptToken>> YScriptLexer::Lex() {
        if (file_path == "<n>")
            return { "<n>" , {} };
        
        src = Filesystem::ReadFileAsStr(file_path);
        src_length = src.length();

        src_index = 0;

        tokens.push_back(YScriptToken(
            YScriptTokenType::START_OF_FILE ,
            0 , 0 , 
            "[< SOF >]"
        ));

        while (!IsEOF()) {
            try {
                if (IsWhitespace(Peek())) {
                    HandleWhitespace();
                    continue;
                }

                if (IsOperator(Peek())) {
                    HandleOperator();
                    continue;
                }

                if (IsNumeric(Peek())) {
                    HandleNumeric();
                    continue;
                }

                if (IsAlpha(Peek())) {
                    HandleAlphaNumeric();
                    continue;
                }
            } catch (const yscript_lexer_error& e) {
                ENGINE_ERROR("Project File Lexer Error :: <{0}>" , e.what());
                return {};
            }
        }
        
        tokens.push_back(YScriptToken(
            YScriptTokenType::END_OF_FILE ,
            0 , 0 , 
            "[< EOF >]"
        ));

        return { src , tokens };
    }
    
}