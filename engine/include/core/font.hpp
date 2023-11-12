#ifndef YE_TEXT_RENDERING_HPP
#define YE_TEXT_RENDERING_HPP

#include <string>
#include <filesystem>

#include <msdfgen/msdfgen.h>
#include <msdfgen/msdfgen-ext.h>
#include <msdf-atlas-gen/GlyphGeometry.h>
#include <msdf-atlas-gen/FontGeometry.h>

#include "defines.hpp"

namespace YE {

    class Texture;

    struct FontData {
        std::vector<msdf_atlas::GlyphGeometry> glyphs;
        msdf_atlas::FontGeometry font_geometry;
    };
    
    class Font {
        std::filesystem::path path;

        msdfgen::FontHandle* font_handle;

        FontData data;
        Texture* atlas_texture = nullptr;

        public:
            Font(const std::filesystem::path& path)
                : path(path) {}
            ~Font() {}
            
            bool Load(bool expensive_coloring = false);
            void Unload();

            inline const FontData& GetGeometryData() const { return data; } 
            inline Texture* GetAtlasTexture() const { return atlas_texture; } 
            inline const std::filesystem::path& Path() const { return path; }
    };

}

#endif // !YE_TEXT_RENDERING_HPP