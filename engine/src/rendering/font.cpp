#include "rendering/font.hpp"

#include <msdf-atlas-gen/AtlasGenerator.h>
#include <msdf-atlas-gen/ImmediateAtlasGenerator.h>
#include <msdf-atlas-gen/BitmapAtlasStorage.h>

#include "log.hpp"
#include "core/defines.hpp"
#include "core/resource_handler.hpp"
#include "rendering/texture.hpp"

namespace YE {
    
    template <typename T , typename S , int N , msdf_atlas::GeneratorFunction<S , N> G>  
    Texture* CreateAtlasTexture(
        float font_size , int width , int height , 
        const FontData& data
    ) {
        msdf_atlas::GeneratorAttributes attrs;
        attrs.config.overlapSupport = true;
        attrs.scanlinePass = true;

        msdf_atlas::ImmediateAtlasGenerator<
            S , N , G , 
            msdf_atlas::BitmapAtlasStorage<T , N>
        > generator(width , height);
        generator.setAttributes(attrs);
        generator.setThreadCount(8);
        generator.generate(data.glyphs.data() , (uint32_t)data.glyphs.size());

        msdfgen::BitmapConstRef<T , N> atlas = (msdfgen::BitmapConstRef<T , N>)generator.atlasStorage();        

        return ynew Texture(
            { atlas.width , atlas.height } , 
            (void*)atlas.pixels
        );
    }
    
    bool Font::Load(bool expensive_coloring) {
        font_handle = msdfgen::loadFont(
            ResourceHandler::Instance()->Freetype() , 
            path.string().c_str()
        );
        if (font_handle == nullptr) {
            LOG_ERROR("Failed to load font: {}" , path.string());
            return false;
        }

        struct CharsetRange {
            uint32_t start;
            uint32_t end;
        };

        /// from imgui_draw.cpp (text rendering line ~2500)
        static const std::vector<CharsetRange> char_sets = {
            { 0x0020 , 0x00FF } // Basic Latin + Latin Supplement
        };

        msdf_atlas::Charset charset;
        for (const auto& range : char_sets) {
            for (uint32_t c = range.start; c <= range.end; ++c)
                charset.add(c);
        }
        
        double font_scale = 1.0;
        data.font_geometry = msdf_atlas::FontGeometry(&data.glyphs);
        data.font_geometry.loadCharset(font_handle , font_scale , charset);

        double em_size = 40.0;

        msdf_atlas::TightAtlasPacker packer;
        packer.setPixelRange(2.0);
        packer.setMiterLimit(1.0);
        packer.setPadding(0);
        packer.setScale(em_size);
        int remaining = packer.pack(data.glyphs.data() , data.glyphs.size());
        if (remaining > 0) {
            LOG_ERROR("Failed to pack {} glyphs" , remaining);
            msdfgen::destroyFont(font_handle);
            font_handle = nullptr;
            return false;
        }

        int width , height;
        packer.getDimensions(width , height);
        em_size = packer.getScale();

#define DEFAULT_ANGLE_THRESHOLD 3.0
#define LCG_MULTIPLIER 6364136223846793005ull
#define LCG_INCREMENT 1442695040888963407ull
#define THREAD_COUNT 8
        
        uint64_t coloring_seed = 0;
        if (expensive_coloring) {
            msdf_atlas::Workload(
                [&gs = data.glyphs , &coloring_seed](int i , int thread_no) -> bool {
                    unsigned long long glyph_seed = (LCG_MULTIPLIER * (coloring_seed ^ i) + LCG_INCREMENT) * !!coloring_seed;
                    gs[i].edgeColoring(msdfgen::edgeColoringInkTrap , DEFAULT_ANGLE_THRESHOLD , glyph_seed);
                    return true;
                } ,
                data.glyphs.size()
            ).finish(THREAD_COUNT);
        } else {
            unsigned long long glyph_seed = coloring_seed;
            for (msdf_atlas::GlyphGeometry& glyph : data.glyphs) {
                glyph_seed *= LCG_MULTIPLIER;
                glyph.edgeColoring(msdfgen::edgeColoringInkTrap , DEFAULT_ANGLE_THRESHOLD , glyph_seed);
            }
        }

        atlas_texture = CreateAtlasTexture<uint8_t , float , 3 , msdf_atlas::msdfGenerator>(
            (float)em_size , width , height , data 
        );

        return atlas_texture != nullptr;
    }

    void Font::Unload() {
        if (font_handle != nullptr)
            msdfgen::destroyFont(font_handle);
    }

}