#ifndef ENGINEY_GLSL_LEXER_HPP
#define ENGINEY_GLSL_LEXER_HPP

#include <string>
#include <vector>
#include <map>
#include <exception>
#include <iostream>
#include <utility>

#include "core/UUID.hpp"

/// \note see engine/include/parsing/glsl/glsl_lexemes.hpp for the definition of the macros below and
///         source of idea for parts of the lexer

#define TYPE(...)
#define KEYWORD(...)
#define OPERATOR(...)
#define TYPENAME(...)

#define STR(x) #x
#define XSTR(x) STR(x)

// inline static constexpr strings (DO NOT INCLUDE ANYWHERE ELSE)
#include "shader_builtins.hpp"

namespace EngineY {
    
    class glsl_lexer_error : public std::exception {
        std::string msg;

        public:
            glsl_lexer_error(const std::string& msg) : msg("[Unknown Position] " + msg) {}
            glsl_lexer_error(uint32_t line , uint32_t col , const std::string& name) 
                : msg("[" + std::to_string(line) + ":" + std::to_string(col) + "] " + name) {}
            ~glsl_lexer_error() {}

            const char* what() const noexcept override { return msg.c_str(); }
    };

    enum class GlslTokenType {
        START_OF_FILE = 0 ,

        // preprocessor
        DEFINE , VERSION , VERSION_NUMBER ,

        // keywords
        TCONST , UNIFORM , BUFFER ,
        SHARED , ATTRIBUTE , VARYING ,
        COHERENT , VOLATILE , RESTRICT ,
        READONLY , WRITEONLY ,
        LAYOUT , CENTROID , FLAT ,
        SMOOTH , NOPERSPECTIVE , PATCH ,
        SAMPLE , INVARIANT , PRECISE ,

        // control flow
        BREAK , CONTINUE , DO ,
        FOR , WHILE , SWITCH ,
        CASE , DEFAULT , IF ,
        ELSE , SUBROUTINE , TIN ,
        TOUT , INOUT , DISCARD , RETURN ,
        
        // types and important type literals
        INT , UINT ,
        TVOID , BOOL , TTRUE ,
        TFALSE , FLOAT , DOUBLE ,
        VEC2 , VEC3 , VEC4 ,
        IVEC2 , IVEC3 , IVEC4 ,
        BVEC2 , BVEC3 , BVEC4 ,
        UVEC2 , UVEC3 , UVEC4 ,
        DVEC2 , DVEC3 , DVEC4 ,
        MAT2 , MAT3 , MAT4 ,
        MAT2X2 , MAT2X3 , MAT2X4 ,
        MAT3X2 , MAT3X3 , MAT3X4 ,
        MAT4X2 , MAT4X3 , MAT4X4 ,
        DMAT2 , DMAT3 , DMAT4 ,
        DMAT2X2 , DMAT2X3 , DMAT2X4 ,
        DMAT3X2 , DMAT3X3 , DMAT3X4 ,
        DMAT4X2 , DMAT4X3 , DMAT4X4 ,
        SAMPLER1D , TEXTURE1D , IMAGE1D ,
        SAMPLER1DSHADOW , 
        SAMPLER1DARRAY , TEXTURE1DARRAY , IMAGE1DARRAY ,
        SAMPLER1DARRAYSHADOW ,
        SAMPLER2D , TEXTURE2D , IMAGE2D ,
        SAMPLER2DSHADOW ,
        SAMPLER2DARRAY , TEXTURE2DARRAY , IMAGE2DARRAY ,
        SAMPLER2DARRAYSHADOW ,
        SAMPLER2DMS , TEXTURE2DMS , IMAGE2DMS ,
        SAMPLER2DMSARRAY , TEXTURE2DMSARRAY , IMAGE2DMSARRAY ,
        SAMPLER2DRECT , TEXTURE2DRECT , IMAGE2DRECT ,
        SAMPLER2DRECTSHADOW ,
        SAMPLER3D , TEXTURE3D , IMAGE3D ,
        SAMPLERCUBE , TEXTURECUBE , IMAGECUBE ,
        SAMPLERCUBESHADOW ,
        SAMPLERCUBEARRAY , TEXTURECUBEARRAY , IMAGECUBEARRAY ,
        SAMPLERCUBEARRAYSHADOW ,
        SAMPLERBUFFER , TEXTUREBUFFER , IMAGEBUFFER ,
        SUBPASSINPUT , SUBPASSINPUTMS ,
        ISAMPLER1D , ITEXTURE1D , IIMAGE1D ,    
        ISAMPLER1DARRAY , ITEXTURE1DARRAY , IIMAGE1DARRAY ,
        ISAMPLER2D , ITEXTURE2D , IIMAGE2D ,
        ISAMPLER2DARRAY , ITEXTURE2DARRAY , IIMAGE2DARRAY ,
        ISAMPLER2DMS , ITEXTURE2DMS , IIMAGE2DMS ,
        ISAMPLER2DMSARRAY , ITEXTURE2DMSARRAY , IIMAGE2DMSARRAY ,
        ISAMPLER2DRECT , ITEXTURE2DRECT , IIMAGE2DRECT ,
        ISAMPLER3D , ITEXTURE3D , IIMAGE3D ,
        ISAMPLERCUBE , ITEXTURECUBE , IIMAGECUBE ,
        ISAMPLERCUBEARRAY , ITEXTURECUBEARRAY , IIMAGECUBEARRAY ,
        ISAMPLERBUFFER , ITEXTUREBUFFER , IIMAGEBUFFER ,
        ISUBPASSINPUT , ISUBPASSINPUTMS ,
        USAMPLER1D , UTEXTURE1D , UIMAGE1D ,
        USAMPLER1DARRAY , UTEXTURE1DARRAY , UIMAGE1DARRAY ,
        USAMPLER2D , UTEXTURE2D , UIMAGE2D ,
        USAMPLER2DARRAY , UTEXTURE2DARRAY , UIMAGE2DARRAY ,
        USAMPLER2DMS , UTEXTURE2DMS , UIMAGE2DMS ,
        USAMPLER2DMSARRAY , UTEXTURE2DMSARRAY , UIMAGE2DMSARRAY ,
        USAMPLER2DRECT , UTEXTURE2DRECT , UIMAGE2DRECT ,
        USAMPLER3D , UTEXTURE3D , UIMAGE3D ,
        USAMPLERCUBE , UTEXTURECUBE , UIMAGECUBE ,
        USAMPLERCUBEARRAY , UTEXTURECUBEARRAY , UIMAGECUBEARRAY ,
        USAMPLERBUFFER , UTEXTUREBUFFER , UIMAGEBUFFER ,
        ATOMIC_UINT , 
        USUBPASSINPUT , USUBPASSINPUTMS ,
        SAMPLER , SAMPLERSHADOW ,

        // more kewyords
        LOWP , MEDIUMP ,
        HIGHP , PRECISION ,
        STRUCT ,

        // identifer
        IDENTIFIER ,

        // operators (precedence order)
        // p1
        OPEN_PAREN , CLOSE_PAREN ,
        // p2
        OPEN_BRACKET , CLOSE_BRACKET /*, OPEN_PAREN , CLOSE_PAREN */ , PERIOD , INCREMENT , DECREMENT ,
        // p3
        /* INCREMENT , DECREMENT , */ ADD , SUB , BIT_NOT , EXCLAMATION ,
        // p4
        MUL , DIV , MOD ,
        // p5
        /* ADD , SUB */
        // p6
        LEFT_SHIFT , RIGHT_SHIFT ,
        // p7
        LESS_THAN , GREATER_THAN , LESS_THAN_EQUAL , GREATER_THAN_EQUAL ,
        // p8
        EQUAL , NOT_EQUAL ,
        // p9
        BIT_AND ,
        // p10
        BIT_XOR ,
        // p11
        BIT_OR ,
        // p12
        AND ,
        // p13
        XOR ,
        // p14
        OR ,
        // p15
        QUESTION_MARK , COLON , SELECTOR ,
        // p16
        ASSIGNMENT , ADD_ASSIGN , SUB_ASSIGN , 
        MUL_ASSIGN , DIV_ASSIGN , MOD_ASSIGN ,
        LEFT_SHIFT_ASSIGN , RIGHT_SHIFT_ASSIGN , AND_ASSIGN ,
        XOR_ASSIGN , OR_ASSIGN ,
        // p17
        COMMA , 
        // no precedence
        OPEN_BRACE , CLOSE_BRACE , SEMICOLON ,

        // literals
        INT_LITERAL , FLOAT_LITERAL , DOUBLE_LITERAL , BOOL_LITERAL , 
        
        // preprocessor
        COMMENT , PREPROCESSOR ,

        // end of file
        END_OF_FILE ,

        // interal use
        PROFILE , EMBED , INCLUDE , USING , VERTEX , FRAGMENT , GEOMETRY ,

        // invalid
        INVALID
    };

    static const std::string GlslTokenTypeStr[] = {
        "START_OF_FILE" ,

        // preprocessor
        "DEFINE" , "VERSION" , "VERSION_NUMBER" ,

        // keywords
        "TCONST" , "UNIFORM" , "BUFFER" ,
        "SHARED" , "ATTRIBUTE" , "VARYING" ,
        "COHERENT" , "VOLATILE" , "RESTRICT" ,
        "READONLY" , "WRITEONLY" ,
        "LAYOUT" , "CENTROID" , "FLAT" ,
        "SMOOTH" , "NOPERSPECTIVE" , "PATCH" ,
        "SAMPLE" , "INVARIANT" , "PRECISE" ,

        // control flow
        "BREAK" , "CONTINUE" , "DO" ,
        "FOR" , "WHILE" , "SWITCH" ,
        "CASE" , "DEFAULT" , "IF" ,
        "ELSE" , "SUBROUTINE" , "TIN" ,
        "TOUT" , "INOUT" , "DISCARD" , "RETURN" ,
        
        // types
        "INT" , "UINT" ,
        "TVOID" , "BOOL" , "TRUE" ,
        "FALSE" , "FLOAT" , "DOUBLE" ,
        "VEC2" , "VEC3" , "VEC4" ,
        "IVEC2" , "IVEC3" , "IVEC4" ,
        "BVEC2" , "BVEC3" , "BVEC4" ,
        "UVEC2" , "UVEC3" , "UVEC4" ,
        "DVEC2" , "DVEC3" , "DVEC4" ,
        "MAT2" , "MAT3" , "MAT4" ,
        "MAT2X2" , "MAT2X3" , "MAT2X4" ,
        "MAT3X2" , "MAT3X3" , "MAT3X4" ,
        "MAT4X2" , "MAT4X3" , "MAT4X4" ,
        "DMAT2" , "DMAT3" , "DMAT4" ,
        "DMAT2X2" , "DMAT2X3" , "DMAT2X4" ,
        "DMAT3X2" , "DMAT3X3" , "DMAT3X4" ,
        "DMAT4X2" , "DMAT4X3" , "DMAT4X4" ,
        "SAMPLER1D" , "TEXTURE1D" , "IMAGE1D" ,
        "SAMPLER1DSHADOW" , 
        "SAMPLER1DARRAY" , "TEXTURE1DARRAY" , "IMAGE1DARRAY" ,
        "SAMPLER1DARRAYSHADOW" ,
        "SAMPLER2D" , "TEXTURE2D" , "IMAGE2D" ,
        "SAMPLER2DSHADOW" ,
        "SAMPLER2DARRAY" , "TEXTURE2DARRAY" , "IMAGE2DARRAY" ,
        "SAMPLER2DARRAYSHADOW" ,
        "SAMPLER2DMS" , "TEXTURE2DMS" , "IMAGE2DMS" ,
        "SAMPLER2DMSARRAY" , "TEXTURE2DMSARRAY" , "IMAGE2DMSARRAY" ,
        "SAMPLER2DRECT" , "TEXTURE2DRECT" , "IMAGE2DRECT" ,
        "SAMPLER2DRECTSHADOW" ,
        "SAMPLER3D" , "TEXTURE3D" , "IMAGE3D" ,
        "SAMPLERCUBE" , "TEXTURECUBE" , "IMAGECUBE" ,
        "SAMPLERCUBESHADOW" ,
        "SAMPLERCUBEARRAY" , "TEXTURECUBEARRAY" , "IMAGECUBEARRAY" ,
        "SAMPLERCUBEARRAYSHADOW" ,
        "SAMPLERBUFFER" , "TEXTUREBUFFER" , "IMAGEBUFFER" ,
        "SUBPASSINPUT" , "SUBPASSINPUTMS" ,
        "ISAMPLER1D" , "ITEXTURE1D" , "IIMAGE1D" ,    
        "ISAMPLER1DARRAY" , "ITEXTURE1DARRAY" , "IIMAGE1DARRAY" ,
        "ISAMPLER2D" , "ITEXTURE2D" , "IIMAGE2D" ,
        "ISAMPLER2DARRAY" , "ITEXTURE2DARRAY" , "IIMAGE2DARRAY" ,
        "ISAMPLER2DMS" , "ITEXTURE2DMS" , "IIMAGE2DMS" ,
        "ISAMPLER2DMSARRAY" , "ITEXTURE2DMSARRAY" , "IIMAGE2DMSARRAY" ,
        "ISAMPLER2DRECT" , "ITEXTURE2DRECT" , "IIMAGE2DRECT" ,
        "ISAMPLER3D" , "ITEXTURE3D" , "IIMAGE3D" ,
        "ISAMPLERCUBE" , "ITEXTURECUBE" , "IIMAGECUBE" ,
        "ISAMPLERCUBEARRAY" , "ITEXTURECUBEARRAY" , "IIMAGECUBEARRAY" ,
        "ISAMPLERBUFFER" , "ITEXTUREBUFFER" , "IIMAGEBUFFER" ,
        "ISUBPASSINPUT" , "ISUBPASSINPUTMS" ,
        "USAMPLER1D" , "UTEXTURE1D" , "UIMAGE1D" ,
        "USAMPLER1DARRAY" , "UTEXTURE1DARRAY" , "UIMAGE1DARRAY" ,
        "USAMPLER2D" , "UTEXTURE2D" , "UIMAGE2D" ,
        "USAMPLER2DARRAY" , "UTEXTURE2DARRAY" , "UIMAGE2DARRAY" ,
        "USAMPLER2DMS" , "UTEXTURE2DMS" , "UIMAGE2DMS" ,
        "USAMPLER2DMSARRAY" , "UTEXTURE2DMSARRAY" , "UIMAGE2DMSARRAY" ,
        "USAMPLER2DRECT" , "UTEXTURE2DRECT" , "UIMAGE2DRECT" ,
        "USAMPLER3D" , "UTEXTURE3D" , "UIMAGE3D" ,
        "USAMPLERCUBE" , "UTEXTURECUBE" , "UIMAGECUBE" ,
        "USAMPLERCUBEARRAY" , "UTEXTURECUBEARRAY" , "UIMAGECUBEARRAY" ,
        "USAMPLERBUFFER" , "UTEXTUREBUFFER" , "UIMAGEBUFFER" ,
        "ATOMIC_UINT" , 
        "USUBPASSINPUT" , "USUBPASSINPUTMS" ,
        "SAMPLER" , "SAMPLERSHADOW" ,

        // more kewyords
        "LOWP" , "MEDIUMP" ,
        "HIGHP" , "PRECISION" ,
        "STRUCT" ,

        // identifer
        "IDENTIFIER" ,

        // operators (precedence order)
        // p1
        "OPEN_PAREN" , "CLOSE_PAREN" ,
        // p2
        "OPEN_BRACKET" , "CLOSE_BRACKET" /*, "OPEN_PAREN" , CLOSE_PAREN "*/ , "PERIOD" , "INCREMENT" , "DECREMENT" ,
        // p3
        /* "INCREMENT" , "DECREMENT" , */ "ADD" , "SUB" , "BIT_NOT" , "EXCLAMATION" ,
        // p4
        "MUL" , "DIV" , "MOD" ,
        // p5
        /* "ADD" , SUB */
        // p6
        "LEFT_SHIFT" , "RIGHT_SHIFT" ,
        // p7
        "LESS_THAN" , "GREATER_THAN" , "LESS_THAN_EQUAL" , "GREATER_THAN_EQUAL" ,
        // p8
        "EQUAL" , "NOT_EQUAL" ,
        // p9
        "BIT_AND" ,
        // p10
        "BIT_XOR" ,
        // p11
        "BIT_OR" ,
        // p12
        "AND" ,
        // p13
        "XOR" ,
        // p14
        "OR" ,
        // p15
        "QUESTION_MARK" , "COLON" , "SELECTOR" ,
        // p16
        "ASSIGNMENT" , "ADD_ASSIGN" , "SUB_ASSIGN" , 
        "MUL_ASSIGN" , "DIV_ASSIGN" , "MOD_ASSIGN" ,
        "LEFT_SHIFT_ASSIGN" , "RIGHT_SHIFT_ASSIGN" , "AND_ASSIGN" ,
        "XOR_ASSIGN" , "OR_ASSIGN" ,
        // p17
        "COMMA" , 
        // no precedence
        "OPEN_BRACE" , "CLOSE_BRACE" , "SEMICOLON" ,

        // literals
        "INT_LITERAL" , "FLOAT_LITERAL" , "DOUBLE_LITERAL" , "BOOL_LITERAL" , 
        
        // preprocessor
        "COMMENT" , "PREPROCESSOR" ,

        // end of file
        "END_OF_FILE" ,

        // interal use
        "PROFILE" , "EMBED" , "INCLUDE" , "USING" , "VERTEX" , "FRAGMENT" , "GEOMETRY" ,

        // invalid
        "INVALID"
    };

    struct GlslToken {
        GlslTokenType type;

        uint32_t line;
        uint32_t column; 
        uint32_t src_index;

        uint32_t start_index;
        uint32_t end_index;

        std::string value;

        GlslToken(GlslTokenType type , uint32_t line , uint32_t column , uint32_t src_index , 
                    uint32_t start_index , uint32_t end_index , const std::string& value) 
            : type(type) , line(line) , column(column) , src_index(src_index) , 
            start_index(start_index) , end_index(end_index) , value(value) {}
    };

    template<typename ostream>
    ostream& operator<<(ostream& os , GlslTokenType type) {
        os << GlslTokenTypeStr[static_cast<uint32_t>(type)];
        return os;
    }

    struct PrintTokenHelper {
        void operator()(GlslTokenType type , const std::string& value) {
            std::cout << type << " " << value << std::endl;
        }

        void operator()(GlslTokenType type) {
            std::cout << type << std::endl;
        }

        void operator()(GlslToken token) {
            (*this)(token.type , token.value);
        }

        void operator()(const std::vector<GlslToken>& tokens) {
            for (const auto& token : tokens) {
                std::cout << token.type << " " << token.value << std::endl;
            }
        }
    };

    class GlslLexer {
        const std::vector<char> operators = {
            '[' , ']' , '.' ,
            '+' , '-' , '~' , '!' ,
            '*' , '/' , '%' ,
            '<' , '>' ,
            '&' , '^' , '|' ,
            '?' , ':' , '=' ,
            ',' , '{' , '}' , ';' ,
            '#' , '(' , ')'
        };

        const std::vector<std::string> keywords = {
            "version" , "core" , "const" , "uniform" , "buffer" , "shared" , "attribute" , "varying" ,
            "coherent" , "volatile" , "restrict" , "readonly" , "writeonly" , "atomic_uint" , "layout" , "centroid" ,
            "flat" , "smooth" , "noperspective" , "patch" , "sample" , "invariant" , "precise" ,
            "break" , "continue" , "do" , "for" , "while" , "switch" , "case" , "default" , "if" , "else" ,
            "subroutine" , "in" , "out" , "inout" , "int" , "void" , "bool" ,
            "true" , "false" , "float" , "double" , "discard" , "return" , "vec2" , "vec3" , "vec4" , "ivec2" , "ivec3" ,
            "ivec4" , "bvec2" , "bvec3" , "bvec4" , "uvec2" , "uvec3" , "uvec4" , "dvec2" , "dvec3" , "dvec4" ,
            "mat2" , "mat3" , "mat4" , "mat2x2" , "mat2x3" , "mat2x4" , "mat3x2" , "mat3x3" ,
            "mat3x4" , "mat4x2" , "mat4x3" , "mat4x4" , "dmat2" , "dmat3" , "dmat4" ,
            "dmat2x2" , "dmat2x3" , "dmat2x4" , "dmat3x2" , "dmat3x3" , "dmat3x4" , "dmat4x2" ,
            "dmat4x3" , "dmat4x4" , "lowp" , "mediump" , "highp" , "precision" , "sampler1D" , "sampler1DShadow" ,
            "sampler1DArray" , "sampler1DArrayShadow" , "isampler1D" , "isampler1DArray" , "usampler1D" ,
            "usampler1DArray" , "sampler2D" , "sampler2DShadow" , "sampler2DArray" ,
            "sampler2DArrayShadow" , "isampler2D" , "isampler2DArray" , "usampler2D" , "usampler2DArray" ,
            "sampler2DRect" , "sampler2DRectShadow" , "isampler2DRect" ,
            "usampler2DRect" , "sampler2DMS" , "isampler2DMS" , "usampler2DMS" , "sampler2DMSArray" , "isampler2DMSArray" , "usampler2DMSArray" ,
            "sampler3D" , "isampler3D" , "usampler3D" , "samplerCube" , "samplerCubeShadow" , "isamplerCube" ,
            "usamplerCube" , "samplerCubeArray" , "samplerCubeArrayShadow" , "isamplerCubeArray" , "usamplerCubeArray" , "samplerBuffer" , "isamplerBuffer" ,
            "usamplerBuffer" , "image1D" , "iimage1D" , "uimage1D" , "image1DArray" , "iimage1DArray" , "uimage1DArray" ,
            "image2D" , "iimage2D" , "uimage2D" , "image2DArray" , "iimage2DArray" , "uimage2DArray" , "image2DRect" , "iimage2DRect" ,
            "uimage2DRect" , "image2DMS" , "iimage2DMS" , "uimage2DMS" , "image2DMSArray" , "iimage2DMSArray" , "uimage2DMSArray" , "image3D" , "iimage3D" ,
            "uimage3D" , "imageCube" , "iimageCube" , "uimageCube" , "imageCubeArray" , "iimageCubeArray" , "uimageCubeArray" , "imageBuffer" ,
            "iimageBuffer" , "uimageBuffer" , "struct" , "include" ,
        };

        std::map<UUID32 , GlslTokenType> keyword_map;
        std::map<UUID32 , GlslTokenType> identifier_map;

        uint32_t src_length = 0;
        uint32_t src_index = 0;
        uint32_t line = 1;
        uint32_t column = 1;
        uint32_t token_start_index = 0;
        uint32_t token_end_index = 0;

        std::string curr_token = "";
        std::string src = "<n>";
        
        std::vector<GlslToken> tokens;

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

        inline GlslTokenType GetTypeForKeyword(const std::string& str) const { return keyword_map.at(Hash::FNV32(str)); }
        inline GlslTokenType GetTypeForIdentifier(const std::string& str) const { return identifier_map.at(Hash::FNV32(str)); }

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

        inline void AddToken(GlslTokenType type) {
            token_end_index = src_index;
            tokens.push_back(GlslToken(
                type , line , column , src_index ,
                token_start_index , token_end_index,
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

        public:
            GlslLexer(const std::string& src)
                : src(src) , src_length(src.length()) {
                InitKeywordMaps();
            }
            ~GlslLexer() {}

            std::pair<std::string , std::vector<GlslToken>> Lex();
    };

}

#endif // !YE_GLSL_LEXER_HPP