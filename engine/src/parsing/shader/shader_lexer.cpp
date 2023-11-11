#include "parsing/shader/shader_lexer.hpp"

#include "log.hpp"
#include "core/hash.hpp"

namespace YE {

    /// for keywords view https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.60.pdf
    void GlslLexer::InitKeywordMaps() {
        keyword_map[Hash::FNV32("version")] = GlslTokenType::VERSION;
        keyword_map[Hash::FNV32("core")] = GlslTokenType::PROFILE;
        keyword_map[Hash::FNV32("const")] = GlslTokenType::TCONST;
        keyword_map[Hash::FNV32("uniform")] = GlslTokenType::UNIFORM;
        keyword_map[Hash::FNV32("buffer")] = GlslTokenType::BUFFER;
        keyword_map[Hash::FNV32("shared")] = GlslTokenType::SHARED;
        keyword_map[Hash::FNV32("attribute")] = GlslTokenType::ATTRIBUTE;
        keyword_map[Hash::FNV32("varying")] = GlslTokenType::VARYING;
        keyword_map[Hash::FNV32("coherent")] = GlslTokenType::COHERENT;
        keyword_map[Hash::FNV32("volatile")] = GlslTokenType::VOLATILE;
        keyword_map[Hash::FNV32("restrict")] = GlslTokenType::RESTRICT;
        keyword_map[Hash::FNV32("readonly")] = GlslTokenType::READONLY;
        keyword_map[Hash::FNV32("writeonly")] = GlslTokenType::WRITEONLY;
        keyword_map[Hash::FNV32("layout")] = GlslTokenType::LAYOUT;
        keyword_map[Hash::FNV32("centroid")] = GlslTokenType::CENTROID;
        keyword_map[Hash::FNV32("flat")] = GlslTokenType::FLAT;
        keyword_map[Hash::FNV32("smooth")] = GlslTokenType::SMOOTH;
        keyword_map[Hash::FNV32("noperspective")] = GlslTokenType::NOPERSPECTIVE;
        keyword_map[Hash::FNV32("patch")] = GlslTokenType::PATCH;
        keyword_map[Hash::FNV32("sample")] = GlslTokenType::SAMPLE;
        keyword_map[Hash::FNV32("invariant")] = GlslTokenType::INVARIANT;
        keyword_map[Hash::FNV32("precise")] = GlslTokenType::PRECISE;

        // control flow
        keyword_map[Hash::FNV32("break")] = GlslTokenType::BREAK;
        keyword_map[Hash::FNV32("continue")] = GlslTokenType::CONTINUE;
        keyword_map[Hash::FNV32("do")] = GlslTokenType::DO;
        keyword_map[Hash::FNV32("for")] = GlslTokenType::FOR;
        keyword_map[Hash::FNV32("while")] = GlslTokenType::WHILE;
        keyword_map[Hash::FNV32("switch")] = GlslTokenType::SWITCH;
        keyword_map[Hash::FNV32("case")] = GlslTokenType::CASE;
        keyword_map[Hash::FNV32("default")] = GlslTokenType::DEFAULT;
        keyword_map[Hash::FNV32("if")] = GlslTokenType::IF;
        keyword_map[Hash::FNV32("else")] = GlslTokenType::ELSE;
        keyword_map[Hash::FNV32("subroutine")] = GlslTokenType::SUBROUTINE;
        keyword_map[Hash::FNV32("in")] = GlslTokenType::TIN;
        keyword_map[Hash::FNV32("out")] = GlslTokenType::TOUT;
        keyword_map[Hash::FNV32("inout")] = GlslTokenType::INOUT;
        keyword_map[Hash::FNV32("discard")] = GlslTokenType::DISCARD;
        keyword_map[Hash::FNV32("return")] = GlslTokenType::RETURN;

        // types and important type literals
        keyword_map[Hash::FNV32("int")] = GlslTokenType::INT;
        keyword_map[Hash::FNV32("uint")] = GlslTokenType::UINT;
        keyword_map[Hash::FNV32("void")] = GlslTokenType::TVOID;
        keyword_map[Hash::FNV32("bool")] = GlslTokenType::BOOL;
        keyword_map[Hash::FNV32("true")] = GlslTokenType::TTRUE;
        keyword_map[Hash::FNV32("false")] = GlslTokenType::TFALSE;
        keyword_map[Hash::FNV32("float")] = GlslTokenType::FLOAT;
        keyword_map[Hash::FNV32("double")] = GlslTokenType::DOUBLE;
        keyword_map[Hash::FNV32("vec2")] = GlslTokenType::VEC2;
        keyword_map[Hash::FNV32("vec3")] = GlslTokenType::VEC3;
        keyword_map[Hash::FNV32("vec4")] = GlslTokenType::VEC4;
        keyword_map[Hash::FNV32("ivec2")] = GlslTokenType::IVEC2;
        keyword_map[Hash::FNV32("ivec3")] = GlslTokenType::IVEC3;
        keyword_map[Hash::FNV32("ivec4")] = GlslTokenType::IVEC4;
        keyword_map[Hash::FNV32("bvec2")] = GlslTokenType::BVEC2;
        keyword_map[Hash::FNV32("bvec3")] = GlslTokenType::BVEC3;
        keyword_map[Hash::FNV32("bvec4")] = GlslTokenType::BVEC4;
        keyword_map[Hash::FNV32("uvec2")] = GlslTokenType::UVEC2;
        keyword_map[Hash::FNV32("uvec3")] = GlslTokenType::UVEC3;
        keyword_map[Hash::FNV32("uvec4")] = GlslTokenType::UVEC4;
        keyword_map[Hash::FNV32("dvec2")] = GlslTokenType::DVEC2;
        keyword_map[Hash::FNV32("dvec3")] = GlslTokenType::DVEC3;
        keyword_map[Hash::FNV32("dvec4")] = GlslTokenType::DVEC4;
        keyword_map[Hash::FNV32("mat2")] = GlslTokenType::MAT2;
        keyword_map[Hash::FNV32("mat3")] = GlslTokenType::MAT3;
        keyword_map[Hash::FNV32("mat4")] = GlslTokenType::MAT4;
        keyword_map[Hash::FNV32("mat2x2")] = GlslTokenType::MAT2X2;
        keyword_map[Hash::FNV32("mat2x3")] = GlslTokenType::MAT2X3;
        keyword_map[Hash::FNV32("mat2x4")] = GlslTokenType::MAT2X4;
        keyword_map[Hash::FNV32("mat3x2")] = GlslTokenType::MAT3X2;
        keyword_map[Hash::FNV32("mat3x3")] = GlslTokenType::MAT3X3;
        keyword_map[Hash::FNV32("mat3x4")] = GlslTokenType::MAT3X4;
        keyword_map[Hash::FNV32("mat4x2")] = GlslTokenType::MAT4X2;
        keyword_map[Hash::FNV32("mat4x3")] = GlslTokenType::MAT4X3;
        keyword_map[Hash::FNV32("mat4x4")] = GlslTokenType::MAT4X4;
        keyword_map[Hash::FNV32("dmat2")] = GlslTokenType::DMAT2;
        keyword_map[Hash::FNV32("dmat3")] = GlslTokenType::DMAT3;
        keyword_map[Hash::FNV32("dmat4")] = GlslTokenType::DMAT4;
        keyword_map[Hash::FNV32("dmat2x2")] = GlslTokenType::DMAT2X2;
        keyword_map[Hash::FNV32("dmat2x3")] = GlslTokenType::DMAT2X3;
        keyword_map[Hash::FNV32("dmat2x4")] = GlslTokenType::DMAT2X4;
        keyword_map[Hash::FNV32("dmat3x2")] = GlslTokenType::DMAT3X2;
        keyword_map[Hash::FNV32("dmat3x3")] = GlslTokenType::DMAT3X3;
        keyword_map[Hash::FNV32("dmat3x4")] = GlslTokenType::DMAT3X4;
        keyword_map[Hash::FNV32("dmat4x2")] = GlslTokenType::DMAT4X2;
        keyword_map[Hash::FNV32("dmat4x3")] = GlslTokenType::DMAT4X3;
        keyword_map[Hash::FNV32("dmat4x4")] = GlslTokenType::DMAT4X4;
        keyword_map[Hash::FNV32("sampler1D")] = GlslTokenType::SAMPLER1D;
        keyword_map[Hash::FNV32("texture1D")] = GlslTokenType::TEXTURE1D;
        keyword_map[Hash::FNV32("image1D")] = GlslTokenType::IMAGE1D;
        keyword_map[Hash::FNV32("sampler1DShadow")] = GlslTokenType::SAMPLER1DSHADOW;
        keyword_map[Hash::FNV32("sampler1DArray")] = GlslTokenType::SAMPLER1DARRAY;
        keyword_map[Hash::FNV32("texture1DArray")] = GlslTokenType::TEXTURE1DARRAY;
        keyword_map[Hash::FNV32("image1DArray")] = GlslTokenType::IMAGE1DARRAY;
        keyword_map[Hash::FNV32("sampler1DArrayShadow")] = GlslTokenType::SAMPLER1DARRAYSHADOW;
        keyword_map[Hash::FNV32("sampler2D")] = GlslTokenType::SAMPLER2D;
        keyword_map[Hash::FNV32("texture2D")] = GlslTokenType::TEXTURE2D;
        keyword_map[Hash::FNV32("image2D")] = GlslTokenType::IMAGE2D;
        keyword_map[Hash::FNV32("sampler2DShadow")] = GlslTokenType::SAMPLER2DSHADOW;
        keyword_map[Hash::FNV32("sampler2DArray")] = GlslTokenType::SAMPLER2DARRAY;
        keyword_map[Hash::FNV32("texture2DArray")] = GlslTokenType::TEXTURE2DARRAY;
        keyword_map[Hash::FNV32("image2DArray")] = GlslTokenType::IMAGE2DARRAY;
        keyword_map[Hash::FNV32("sampler2DArrayShadow")] = GlslTokenType::SAMPLER2DARRAYSHADOW;
        keyword_map[Hash::FNV32("sampler2DMS")] = GlslTokenType::SAMPLER2DMS;
        keyword_map[Hash::FNV32("texture2DMS")] = GlslTokenType::TEXTURE2DMS;
        keyword_map[Hash::FNV32("image2DMS")] = GlslTokenType::IMAGE2DMS;
        keyword_map[Hash::FNV32("sampler2DMSArray")] = GlslTokenType::SAMPLER2DMSARRAY;
        keyword_map[Hash::FNV32("texture2DMSArray")] = GlslTokenType::TEXTURE2DMSARRAY;
        keyword_map[Hash::FNV32("image2DMSArray")] = GlslTokenType::IMAGE2DMSARRAY;
        keyword_map[Hash::FNV32("sampler2DRect")] = GlslTokenType::SAMPLER2DRECT;
        keyword_map[Hash::FNV32("texture2DRect")] = GlslTokenType::TEXTURE2DRECT;
        keyword_map[Hash::FNV32("image2DRect")] = GlslTokenType::IMAGE2DRECT;
        keyword_map[Hash::FNV32("sampler2DRectShadow")] = GlslTokenType::SAMPLER2DRECTSHADOW;
        keyword_map[Hash::FNV32("sampler3D")] = GlslTokenType::SAMPLER3D;
        keyword_map[Hash::FNV32("texture3D")] = GlslTokenType::TEXTURE3D;
        keyword_map[Hash::FNV32("image3D")] = GlslTokenType::IMAGE3D;
        keyword_map[Hash::FNV32("samplerCube")] = GlslTokenType::SAMPLERCUBE;
        keyword_map[Hash::FNV32("textureCube")] = GlslTokenType::TEXTURECUBE;
        keyword_map[Hash::FNV32("imageCube")] = GlslTokenType::IMAGECUBE;
        keyword_map[Hash::FNV32("samplerCubeShadow")] = GlslTokenType::SAMPLERCUBESHADOW;
        keyword_map[Hash::FNV32("samplerCubeArray")] = GlslTokenType::SAMPLERCUBEARRAY;
        keyword_map[Hash::FNV32("textureCubeArray")] = GlslTokenType::TEXTURECUBEARRAY;
        keyword_map[Hash::FNV32("imageCubeArray")] = GlslTokenType::IMAGECUBEARRAY;
        keyword_map[Hash::FNV32("samplerCubeArrayShadow")] = GlslTokenType::SAMPLERCUBEARRAYSHADOW;
        keyword_map[Hash::FNV32("samplerBuffer")] = GlslTokenType::SAMPLERBUFFER;
        keyword_map[Hash::FNV32("textureBuffer")] = GlslTokenType::TEXTUREBUFFER;
        keyword_map[Hash::FNV32("imageBuffer")] = GlslTokenType::IMAGEBUFFER;
        keyword_map[Hash::FNV32("subpassInput")] = GlslTokenType::SUBPASSINPUT;
        keyword_map[Hash::FNV32("subpassInputMS")] = GlslTokenType::SUBPASSINPUTMS;
        keyword_map[Hash::FNV32("isampler1D")] = GlslTokenType::ISAMPLER1D;
        keyword_map[Hash::FNV32("iimage1D")] = GlslTokenType::IIMAGE1D;
        keyword_map[Hash::FNV32("isampler1DArray")] = GlslTokenType::ISAMPLER1DARRAY;
        keyword_map[Hash::FNV32("iimage1DArray")] = GlslTokenType::IIMAGE1DARRAY;
        keyword_map[Hash::FNV32("isampler2D")] = GlslTokenType::ISAMPLER2D;
        keyword_map[Hash::FNV32("iimage2D")] = GlslTokenType::IIMAGE2D;
        keyword_map[Hash::FNV32("isampler2DArray")] = GlslTokenType::ISAMPLER2DARRAY;
        keyword_map[Hash::FNV32("itexture2DArray")] = GlslTokenType::ITEXTURE2DARRAY;
        keyword_map[Hash::FNV32("iimage2DArray")] = GlslTokenType::IIMAGE2DARRAY;
        keyword_map[Hash::FNV32("isampler2DMS")] = GlslTokenType::ISAMPLER2DMS;
        keyword_map[Hash::FNV32("itexture2DMS")] = GlslTokenType::ITEXTURE2DMS;
        keyword_map[Hash::FNV32("iimage2DMS")] = GlslTokenType::IIMAGE2DMS;
        keyword_map[Hash::FNV32("isampler2DMSArray")] = GlslTokenType::ISAMPLER2DMSARRAY;
        keyword_map[Hash::FNV32("itexture2DMSArray")] = GlslTokenType::ITEXTURE2DMSARRAY;
        keyword_map[Hash::FNV32("iimage2DMSArray")] = GlslTokenType::IIMAGE2DMSARRAY;
        keyword_map[Hash::FNV32("isampler2DRect")] = GlslTokenType::ISAMPLER2DRECT;
        keyword_map[Hash::FNV32("itexture2DRect")] = GlslTokenType::ITEXTURE2DRECT;
        keyword_map[Hash::FNV32("iimage2DRect")] = GlslTokenType::IIMAGE2DRECT;
        keyword_map[Hash::FNV32("isampler3D")] = GlslTokenType::ISAMPLER3D;
        keyword_map[Hash::FNV32("itexture3D")] = GlslTokenType::ITEXTURE3D;
        keyword_map[Hash::FNV32("iimage3D")] = GlslTokenType::IIMAGE3D;
        keyword_map[Hash::FNV32("isamplerCube")] = GlslTokenType::ISAMPLERCUBE;
        keyword_map[Hash::FNV32("itextureCube")] = GlslTokenType::ITEXTURECUBE;
        keyword_map[Hash::FNV32("iimageCube")] = GlslTokenType::IIMAGECUBE;
        keyword_map[Hash::FNV32("isamplerCubeArray")] = GlslTokenType::ISAMPLERCUBEARRAY;
        keyword_map[Hash::FNV32("itextureCubeArray")] = GlslTokenType::ITEXTURECUBEARRAY;
        keyword_map[Hash::FNV32("iimageCubeArray")] = GlslTokenType::IIMAGECUBEARRAY;
        keyword_map[Hash::FNV32("isamplerBuffer")] = GlslTokenType::ISAMPLERBUFFER;
        keyword_map[Hash::FNV32("itextureBuffer")] = GlslTokenType::ITEXTUREBUFFER;
        keyword_map[Hash::FNV32("iimageBuffer")] = GlslTokenType::IIMAGEBUFFER;
        keyword_map[Hash::FNV32("isubpassInput")] = GlslTokenType::ISUBPASSINPUT;
        keyword_map[Hash::FNV32("isubpassInputMS")] = GlslTokenType::ISUBPASSINPUTMS;
        keyword_map[Hash::FNV32("usampler1D")] = GlslTokenType::USAMPLER1D;
        keyword_map[Hash::FNV32("utexture1D")] = GlslTokenType::UTEXTURE1D;
        keyword_map[Hash::FNV32("uimage1D")] = GlslTokenType::UIMAGE1D;
        keyword_map[Hash::FNV32("usampler1DArray")] = GlslTokenType::USAMPLER1DARRAY;
        keyword_map[Hash::FNV32("utexture1DArray")] = GlslTokenType::UTEXTURE1DARRAY;
        keyword_map[Hash::FNV32("uimage1DArray")] = GlslTokenType::UIMAGE1DARRAY;
        keyword_map[Hash::FNV32("usampler2D")] = GlslTokenType::USAMPLER2D;
        keyword_map[Hash::FNV32("utexture2D")] = GlslTokenType::UTEXTURE2D;
        keyword_map[Hash::FNV32("uimage2D")] = GlslTokenType::UIMAGE2D;
        keyword_map[Hash::FNV32("usampler2DArray")] = GlslTokenType::USAMPLER2DARRAY;
        keyword_map[Hash::FNV32("utexture2DArray")] = GlslTokenType::UTEXTURE2DARRAY;
        keyword_map[Hash::FNV32("uimage2DArray")] = GlslTokenType::UIMAGE2DARRAY;
        keyword_map[Hash::FNV32("usampler2DMS")] = GlslTokenType::USAMPLER2DMS;
        keyword_map[Hash::FNV32("utexture2DMS")] = GlslTokenType::UTEXTURE2DMS;
        keyword_map[Hash::FNV32("uimage2DMS")] = GlslTokenType::UIMAGE2DMS;
        keyword_map[Hash::FNV32("usampler2DMSArray")] = GlslTokenType::USAMPLER2DMSARRAY;
        keyword_map[Hash::FNV32("utexture2DMSArray")] = GlslTokenType::UTEXTURE2DMSARRAY;
        keyword_map[Hash::FNV32("uimage2DMSArray")] = GlslTokenType::UIMAGE2DMSARRAY;
        keyword_map[Hash::FNV32("usampler2DRect")] = GlslTokenType::USAMPLER2DRECT;
        keyword_map[Hash::FNV32("utexture2DRect")] = GlslTokenType::UTEXTURE2DRECT;
        keyword_map[Hash::FNV32("uimage2DRect")] = GlslTokenType::UIMAGE2DRECT;
        keyword_map[Hash::FNV32("usampler3D")] = GlslTokenType::USAMPLER3D;
        keyword_map[Hash::FNV32("utexture3D")] = GlslTokenType::UTEXTURE3D;
        keyword_map[Hash::FNV32("uimage3D")] = GlslTokenType::UIMAGE3D;
        keyword_map[Hash::FNV32("usamplerCube")] = GlslTokenType::USAMPLERCUBE;
        keyword_map[Hash::FNV32("utextureCube")] = GlslTokenType::UTEXTURECUBE;
        keyword_map[Hash::FNV32("uimageCube")] = GlslTokenType::UIMAGECUBE;
        keyword_map[Hash::FNV32("usamplerCubeArray")] = GlslTokenType::USAMPLERCUBEARRAY;
        keyword_map[Hash::FNV32("utextureCubeArray")] = GlslTokenType::UTEXTURECUBEARRAY;
        keyword_map[Hash::FNV32("uimageCubeArray")] = GlslTokenType::UIMAGECUBEARRAY;
        keyword_map[Hash::FNV32("usamplerBuffer")] = GlslTokenType::USAMPLERBUFFER;
        keyword_map[Hash::FNV32("utextureBuffer")] = GlslTokenType::UTEXTUREBUFFER;
        keyword_map[Hash::FNV32("uimageBuffer")] = GlslTokenType::UIMAGEBUFFER;
        keyword_map[Hash::FNV32("atomic_uint")] = GlslTokenType::ATOMIC_UINT;
        keyword_map[Hash::FNV32("usubpassInput")] = GlslTokenType::USUBPASSINPUT;
        keyword_map[Hash::FNV32("usubpassInputMS")] = GlslTokenType::USUBPASSINPUTMS;
        keyword_map[Hash::FNV32("sampler")] = GlslTokenType::SAMPLER;
        keyword_map[Hash::FNV32("samplerShadow")] = GlslTokenType::SAMPLERSHADOW;


        // more keywords
        keyword_map[Hash::FNV32("lowp")] = GlslTokenType::LOWP;
        keyword_map[Hash::FNV32("mediump")] = GlslTokenType::MEDIUMP;
        keyword_map[Hash::FNV32("highp")] = GlslTokenType::HIGHP;
        keyword_map[Hash::FNV32("precision")] = GlslTokenType::PRECISION;
        keyword_map[Hash::FNV32("struct")] = GlslTokenType::STRUCT;
        
        // internal use
        keyword_map[Hash::FNV32("include")] = GlslTokenType::INCLUDE;
    }

    void GlslLexer::HandleWhitespace() {
        while (IsWhitespace(Peek())) {
            if (Peek() == '\n') {
                NewLine();
            } else {
                Consume();
            }

            if (IsEOF()) break;
        }
    }

    void GlslLexer::HandleOperator() {
        char c = Peek();
        Advance();
        switch (c) {
            case '(': AddToken(GlslTokenType::OPEN_PAREN); break;
            case ')': AddToken(GlslTokenType::CLOSE_PAREN); break;
            case '[': AddToken(GlslTokenType::OPEN_BRACKET); break;
            case ']': AddToken(GlslTokenType::CLOSE_BRACKET); break;
            case '.': AddToken(GlslTokenType::PERIOD); break;
            case '+':
                if (Check('+')) {
                    Advance();
                    AddToken(GlslTokenType::INCREMENT);
                } else if (Check('=')) {
                    Advance();
                    AddToken(GlslTokenType::ADD_ASSIGN);
                } else {
                    AddToken(GlslTokenType::ADD);
                }
            break;
            case '-':
                if (Check('-')) {
                    Advance();
                    AddToken(GlslTokenType::DECREMENT);
                } else if (Check('=')) {
                    Advance();
                    AddToken(GlslTokenType::SUB_ASSIGN);
                } else {
                    AddToken(GlslTokenType::SUB);
                }
            break;
            case '~': AddToken(GlslTokenType::BIT_NOT); break;
            case '!':
                if (!Check('=')) {
                    AddToken(GlslTokenType::EXCLAMATION);
                } else {
                    Advance();
                    AddToken(GlslTokenType::NOT_EQUAL);
                }
            case '*':
                if (!Check('=')) {
                    AddToken(GlslTokenType::MUL);
                } else {
                    Advance();
                    AddToken(GlslTokenType::MUL_ASSIGN);
                }
            break;
            case '/':
                if (Check('=')) {
                    Advance();
                    AddToken(GlslTokenType::DIV_ASSIGN);
                } else if (Check('/')) {
                    HandleComment();
                } else {
                    AddToken(GlslTokenType::DIV);
                }
            break;
            case '%':
                if (!Check('=')) {
                    AddToken(GlslTokenType::MOD);
                } else {
                    Advance();
                    AddToken(GlslTokenType::MOD_ASSIGN);
                }
            break;
            case '<':
                if (Check('<')) {
                    Advance();
                    AddToken(Match('=') ? GlslTokenType::LEFT_SHIFT_ASSIGN : GlslTokenType::LEFT_SHIFT);
                } else if (Check('=')) {
                    Advance();
                    AddToken(GlslTokenType::LESS_THAN_EQUAL);
                } else {
                    AddToken(GlslTokenType::LESS_THAN);
                }
            break;
            case '>':
                if (Check('>')) {
                    Advance();
                    AddToken(Match('=') ? GlslTokenType::RIGHT_SHIFT_ASSIGN : GlslTokenType::RIGHT_SHIFT);
                } else if (Check('=')) {
                    Advance();
                    AddToken(GlslTokenType::GREATER_THAN_EQUAL);
                } else {
                    AddToken(GlslTokenType::GREATER_THAN);
                }
            break;
            case '=':
                if (Check('=')) {
                    Advance();
                    AddToken(GlslTokenType::EQUAL);
                } else {
                    AddToken(GlslTokenType::ASSIGNMENT);
                }
            break;
            case '&':
                if (Check('&')) {
                    Advance();
                    AddToken(GlslTokenType::AND);
                } else if (Check('=')) {
                    Advance();
                    AddToken(GlslTokenType::AND_ASSIGN);
                } else {
                    AddToken(GlslTokenType::BIT_AND);
                }
            break;
            case '^':
                if (Check('=')) {
                    Advance();
                    AddToken(GlslTokenType::XOR_ASSIGN);
                } else {
                    AddToken(GlslTokenType::BIT_XOR);
                }
            break;
            case '|':
                if (Check('|')) {
                    Advance();
                    AddToken(GlslTokenType::OR);
                } else if (Check('=')) {
                    Advance();
                    AddToken(GlslTokenType::OR_ASSIGN);
                } else {
                    AddToken(GlslTokenType::BIT_OR);
                }
            break;
            case '?': AddToken(GlslTokenType::QUESTION_MARK); break;
            case ':': AddToken(GlslTokenType::COLON); break;
            case ',': AddToken(GlslTokenType::COMMA); break;
            case '{': AddToken(GlslTokenType::OPEN_BRACE); break;
            case '}': AddToken(GlslTokenType::CLOSE_BRACE); break;
            case ';': AddToken(GlslTokenType::SEMICOLON); break;
            case '#': AddToken(GlslTokenType::PREPROCESSOR); break;
            default:
                throw glsl_lexer_error(line , column , "Undefine Behavior In Glsl Lexer");
        }
    }

    void GlslLexer::HandleComment() {
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
                if (Peek() == '*' && PeekNext() == '\n') break;
                Advance();
            }

            if (Peek() != '*')
                throw glsl_lexer_error(line , column , "Unterminated block comment");
            Advance();

            if (Peek() != '/')
                throw glsl_lexer_error(line , column , "Undefine Behavior In Glsl Lexer");
            Advance();
        }

        DiscardToken();
        // AddToken(GlslTokenType::COMMENT) // might want to show comments in editor??
    }

    void GlslLexer::HandleNumeric() {
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
            AddToken(GlslTokenType::FLOAT_LITERAL);
        } else if (Peek() == 'x' && IsNumeric(PeekNext())) {
            Advance();
            while (IsNumeric(Peek())) {
                Advance();
                if (IsEOF()) break;
            }
            AddToken(GlslTokenType::INT_LITERAL);
        } else {
            AddToken(GlslTokenType::INT_LITERAL);
        }
    }

    void GlslLexer::HandleAlphaNumeric() {
        while (IsAlphaNumeric(Peek())) {
            Advance();
            if (IsEOF()) break;
        }

        if (IsKeyword(curr_token)) {
            AddToken(keyword_map[Hash::FNV32(curr_token)]);
        } else {
            AddToken(GlslTokenType::IDENTIFIER);
        }
    }

    std::pair<std::string , std::vector<GlslToken>> GlslLexer::Lex() {
        if (this->src == "<n>" || this->src.length() == 0)
            return { "<n>" , {} };
        
        tokens.push_back(GlslToken(
            GlslTokenType::START_OF_FILE ,
            0 , 0 , 0 , 0 , 0 ,
            "[<SOF>]"
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
            } catch (const glsl_lexer_error& e) {
                ENGINE_ERROR("GLSL Lexer Error :: <{0}>" , e.what());
                return {};
            }
        }
        tokens.push_back(GlslToken(
            GlslTokenType::END_OF_FILE ,
            0 , 0 , 0 , 0 , 0 ,
            "[<EOF>]"
        ));

        return { src , tokens };
    }   

}