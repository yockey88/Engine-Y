#ifndef YE_YSCRIPT_LEXER_HPP
#define YE_YSCRIPT_LEXER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

#include <magic_enum/magic_enum.hpp>

#include "core/hash.hpp"
#include "core/UUID.hpp"

namespace YE {

    class yscript_lexer_error : public std::exception {
        std::string msg;
        
        public:
            yscript_lexer_error(const std::string msg , uint32_t line , uint32_t col)
                : msg("Y Script Lexer Error at [" + std::to_string(line) + " , " + std::to_string(col) + "] :: " + msg) {}
            ~yscript_lexer_error() {}

            const char* what() const noexcept override { return msg.c_str(); }
    };

    enum class YScriptTokenType {
        START_OF_FILE ,

        // keywords 
        PROJECT , NAME , AUTHOR , VERSION , DESCRIPTION , RESOURCES , PATH ,
        WINDOW , TITLE , MIN_SCALE , CLEAR_COLOR , FLAGS , COLOR_BITS , STENCIL_SIZE ,
            MULTISAMPLE_BUFFERS , MULTISAMPLE_SAMPLES , FULLSCREEN , VSYNC , RENDERING_TO_SCREEN ,
            ACCELERATED_VISUAL ,
        NODE , 
        SCENE ,
        ENTITY ,
        TRANSFORM , POSITION , ROTATION , SCALE ,
        RENDERABLE , TEXTURED_RENDERABLE , RENDERABLE_MODEL , MESH , TEXTURE , SHADER , MODEL ,
        POINT_LIGHT , COLOR , AMBIENT , DIFFUSE , SPECULAR , CONSTANT , LINEAR , QUADRATIC ,
        CAMERA , ORTHOGRAPHIC , PERSPECTIVE , FRONT , UP , RIGHT , WORLD_UP , EULER_ANGLES , 
            VIEWPORT , CLIP , SPEED , SENSITIVITY , FOV , ZOOM ,
        SCRIPT , NATIVE_SCRIPT , OBJECT ,
        PHYSICS_BODY , PHYSICS_BODY_TYPE ,
        BOX_COLLIDER , SPHERE_COLLIDER , CAPSULE_COLLIDER , MODEL_COLLIDER , RADIUS , HEIGHT ,
        TRUE_TKN , FALSE_TKN ,
        FN ,

        IDENTIFIER ,
        
        LESS_THAN , GREATER_THAN ,
        OPEN_PAREN , CLOSE_PAREN ,
        OPEN_BRACE , CLOSE_BRACE ,
        OPEN_BRACKET , CLOSE_BRACKET ,
        COMMA , COLON , COLON_COLON , PERIOD ,
        QUOTE , DOUBLE_QUOTE ,
        BACKWARD_SLASH ,
        EQUALS , 
        EQUALS_EQUALS , LESS_THAN_EQUALS , GREATER_THAN_EQUALS ,
        MINUS , PLUS , ASTERISK , FORWARD_SLASH , PERCENT ,
        MINUS_MINUS , PLUS_PLUS ,
        MINUS_EQUALS , PLUS_EQUALS , ASTERISK_EQUALS , FORWARD_SLASH_EQUALS , PERCENT_EQUALS ,
        EXCLAMATION , QUESTION ,
        EXCLAMATION_EQUALS ,  // ?= Maybe for some sort of error checking >> QUESTION_EQUALS ,
        LOGICAL_AND , LOGICAL_OR ,
        SEMICOLON ,
        HASH , 
        DOLLAR , EMPTY_VALUE ,
        UNDERSCORE ,

        STRING , CHARACTER ,
        FLOAT ,
        
        COMMENT ,

        END_OF_FILE ,

        // Error/Special tokens
        MAIN_FN , // not needed, but will override script loading if found 
        BLANK , UNKNOWN
    };

    template<typename ostream>
    ostream& operator<<(ostream& os , YScriptTokenType type) {
        os << magic_enum::enum_name(type);
        return os;
    }

    struct YScriptToken {
        YScriptTokenType type;
        uint32_t line;
        uint32_t col;
        std::string value;

        YScriptToken() {}
        YScriptToken(YScriptTokenType type , uint32_t line , uint32_t col , std::string value)
            : type(type) , line(line) , col(col) , value(value) {}
        inline bool operator==(const YScriptToken& other) const {
            return (type == other.type) && (line == other.line) && 
                   (col == other.col) && (value == other.value);
        }
        inline bool operator!=(const YScriptToken& other) const {
            return !(*this == other);
        }
    };

    struct YScriptTokenPrinter {
        void operator()(YScriptTokenType type) {
            std::cout << type << "\n";
        }

        void operator()(YScriptToken& token , bool position = false) {
            std::cout << token.type << " : " << token.value;
            if (position)
                std::cout << " [" << token.line << " , " << token.col << "]";
            std::cout << "\n";
        }
        
        void operator()(const YScriptToken& token , bool position = false) {
            std::cout << token.type << " : " << token.value;
            if (position)
                std::cout << " [" << token.line << " , " << token.col << "]";
            std::cout << "\n";
        }

        void operator()(const std::vector<YScriptToken>& tokens , bool position = false) {
            for (auto& token : tokens) {
                std::cout << token.type << " : " << token.value;
                if (position)
                    std::cout << " [" << token.line << " , " << token.col << "]";
                std::cout << "\n";
            }
        }
    };

    class YScriptLexer {
        const std::vector<char> operators = {
            '<'  , '>'  ,
            '('  , ')'  ,
            '{'  , '}'  ,
            '['  , ']'  ,
            ','  , ':'  , '.' ,
            '\'' , '"'  ,
            '\\' ,
            '='  , 
            '-'  , '+'  , '*' , '/' , '%' , 
            '!'  , '?'  , 
            '&'  , '|'  ,
            ';'  , 
            '#'  ,
            '$'  , 
            '_'
        };

        const std::vector<std::string> keywords = {
            /* node type | properties for node type (not exclusive) */
            "project" , "name" , "author" , "version" , "description" , "resources" , "path" ,
            "window" , "title" , "min_scale" , "clear_color" , "flags" , "color_bits" , "stencil_size" ,
                "multisample_buffers" , "multisample_samples" , "fullscreen" , "vsync" , "rendering_to_screen" ,
                "accelerated_visual" ,
            "node" ,
            "scene" ,
            "entity" ,
            "transform" , "position" , "rotation" , "scale" ,
            "renderable" , "textured_renderable" , "renderable_model" , "mesh" , "texture" , "shader" , "model" ,
            "point_light" , "color" , "ambient" , "diffuse" , "specular" , "constant" , "linear" , "quadratic" ,
            "camera" , "orthographic" , "perspective" , "front" , "up" , "right" , "world_up" , "euler_angles" , 
                "viewport" , "clip" , "speed" , "sensitivity" , "fov" , "zoom" ,
            "script" , "native_script" , "object" ,
            "physics_body" , "body_type" ,
            "box_collider" , "sphere_collider" , "capsule_collider" , "model_collider" , "radius" , "height" ,
            "true" , "false" ,
            "fn" , 
            "main" , // not needed, but will override script loading if found
        };

        std::vector<YScriptToken> tokens{};
        std::vector<YScriptToken> comments{};
        std::map<UUID32 , YScriptTokenType> keyword_map;

        uint32_t src_length = 0;
        uint32_t src_index = 0;
        uint32_t line = 1;
        uint32_t column = 1;
        uint32_t token_start_index = 0;
        uint32_t token_end_index = 0;

        std::string curr_token = "";
        std::string src = "<n>";
        std::string file_path = "<n>";

        inline bool IsNumeric(char c) { return (c >= '0' && c <= '9'); }

        inline bool IsAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }

        inline bool IsAlphaNumeric(char c) { return (IsAlpha(c) || IsNumeric(c) || c == '_'); }

        inline bool IsOperator(char c) const {
            return (std::find(operators.begin() , operators.end() , c) != operators.end());
        } 

        inline bool IsWhitespace(char c) {
            return (c == ' ') || (c == '\t') || (c == '\f') || (c == '\v') || (c == '\n');
        }

        inline bool IsKeyword(const std::string& str) const {
            return (std::find(keywords.begin() , keywords.end() , str) != keywords.end());
        }

        inline YScriptTokenType GetTypeForKeyword(const std::string& str) const { return keyword_map.at(Hash::FNV32(str)); }

        inline void NewLine(bool iterate_src_index = true) {
            ++line;
            column = 1;
            if (iterate_src_index) 
                ++src_index;
        }

        inline void Consume() {
            ++src_index;
            ++column;
            ++token_end_index;
        }

        inline void Advance() {
            curr_token += src[src_index];
            Consume();
        }

        inline void DiscardToken() { 
            curr_token = "";
            token_start_index = src_index;
            token_end_index = src_index;    
        }

        inline void AddToken(YScriptTokenType type) {
            token_end_index = src_index;
            tokens.push_back(YScriptToken(
                type ,
                line , column ,
                curr_token
            ));
            DiscardToken();
        }

        inline bool IsEOF() const { return (src_index >= src_length); }

        inline char Peek() const {
            if (IsEOF())
                return '\0';
            return src[src_index];
        }

        inline char PeekNext() const {
            if (src_index + 1 >= src_length)
                return '\0';
            return src[src_index + 1];
        }

        inline bool Check(char c) const {
            if (IsEOF())
                return false;
            return (Peek() == c);
        }

        inline bool CheckNext(char c) const {
            if (src_index + 1 >= src_length)
                return false;
            return (PeekNext() == c);
        }

        inline bool Match(char c) {
            if (!Check(c)) return false;
            Advance();
            return true;
        }

        void InitKeywordMaps();

        void HandleWhitespace();
        void HandleOperator();
        void HandleComment();
        void HandleNumeric();
        void HandleAlphaNumeric();
        void HandleCharacter();
        void HandleString();

        public:
            YScriptLexer(const std::string& file_path) 
                    : file_path(file_path) {
                InitKeywordMaps();
            }
            ~YScriptLexer() {}

            std::pair<std::string , std::vector<YScriptToken>> Lex();
    };

}

#endif // !YE_YSCRIPT_LEXER_HPP