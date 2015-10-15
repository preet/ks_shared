#ifndef KS_IMAGE_PNG_HPP
#define KS_IMAGE_PNG_HPP

// lodepng
#include <lodepng/lodepng.h>

namespace ks
{
    // ============================================================= //
    // ============================================================= //

    namespace image_detail
    {
        template<u8 Channels, u8 BitDepth>
        struct png_data
        {
            template<typename Pixel>
            static void assign(std::vector<u8> const &,
                               std::vector<Pixel> &)
            {
                // do nothing
            }
        };

        template<>
        struct png_data<1,1>
        {
            template<typename Pixel>
            static void assign(std::vector<u8> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                for(u8 const byte : list_bytes) {
                    list_pixels.emplace_back(Pixel{ byte & 1 });
                    list_pixels.emplace_back(Pixel{ byte & 2 });
                    list_pixels.emplace_back(Pixel{ byte & 4 });
                    list_pixels.emplace_back(Pixel{ byte & 8 });
                    list_pixels.emplace_back(Pixel{ byte & 16 });
                    list_pixels.emplace_back(Pixel{ byte & 32 });
                    list_pixels.emplace_back(Pixel{ byte & 64 });
                    list_pixels.emplace_back(Pixel{ byte & 128 });
                }
            }
        };

        template<>
        struct png_data<1,2>
        {
            template<typename Pixel>
            static void assign(std::vector<u8> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                for(u8 const byte : list_bytes) {
                    list_pixels.emplace_back(Pixel{ byte & 3 });
                    list_pixels.emplace_back(Pixel{ byte & 12 });
                    list_pixels.emplace_back(Pixel{ byte & 48 });
                    list_pixels.emplace_back(Pixel{ byte & 192 });
                }
            }
        };

        template<>
        struct png_data<1,4>
        {
            template<typename Pixel>
            static void assign(std::vector<u8> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                for(u8 const byte : list_bytes) {
                    list_pixels.emplace_back(Pixel{ byte & 15 });
                    list_pixels.emplace_back(Pixel{ byte & 240 });
                }
            }
        };

        template<>
        struct png_data<1,8>
        {
            template<typename Pixel>
            static void assign(std::vector<u8> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                for(u8 const byte : list_bytes) {
                    list_pixels.emplace_back(Pixel{ byte });
                }
            }
        };

        template<>
        struct png_data<1,16>
        {
            template<typename Pixel>
            static void assign(std::vector<u8> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                // png data is in big endian
                for(size_t i=0; i < list_bytes.size(); i+=2) {
                    // TODO check platform endianess! (currently LE only)
                    list_pixels.emplace_back(Pixel {
                        (static_cast<u16>(list_bytes[i+1]) << 8) | list_bytes[i]
                    });
                }
            }
        };

        template<>
        struct png_data<2,8>
        {
            template<typename Pixel>
            static void assign(std::vector<u8> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                for(size_t i=0; i < list_bytes.size(); i+=2) {
                    list_pixels.emplace_back(Pixel {
                        list_bytes[i],
                        list_bytes[i+1]
                    });
                }
            }
        };

        template<>
        struct png_data<2,16>
        {
            template<typename Pixel>
            static void assign(std::vector<u8> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                for(size_t i=0; i < list_bytes.size(); i+=4) {
                    // TODO check platform endianess! (currently LE only)
                    list_pixels.emplace_back(Pixel {
                        ((static_cast<u16>(list_bytes[i+1]) << 8) | list_bytes[i+0]),
                        ((static_cast<u16>(list_bytes[i+3]) << 8) | list_bytes[i+2])
                    });
                }
            }
        };

        template<>
        struct png_data<3,8>
        {
            template<typename Pixel>
            static void assign(std::vector<u8> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                for(size_t i=0; i < list_bytes.size(); i+=3) {
                    list_pixels.emplace_back(Pixel {
                        list_bytes[i+0],
                        list_bytes[i+1],
                        list_bytes[i+2]
                    });
                }
            }
        };

        template<>
        struct png_data<3,16>
        {
            template<typename Pixel>
            static void assign(std::vector<u8> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                for(size_t i=0; i < list_bytes.size(); i+=6) {
                    // TODO check platform endianess! (currently LE only)
                    list_pixels.emplace_back(Pixel {
                        ((static_cast<u16>(list_bytes[i+1]) << 8) | list_bytes[i+0]),
                        ((static_cast<u16>(list_bytes[i+3]) << 8) | list_bytes[i+2]),
                        ((static_cast<u16>(list_bytes[i+5]) << 8) | list_bytes[i+4])
                    });
                }
            }
        };

        template<>
        struct png_data<4,8>
        {
            template<typename Pixel>
            static void assign(std::vector<u8> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                for(size_t i=0; i < list_bytes.size(); i+=4) {
                    list_pixels.emplace_back(Pixel {
                        list_bytes[i+0],
                        list_bytes[i+1],
                        list_bytes[i+2],
                        list_bytes[i+3]
                    });
                }
            }
        };

        template<>
        struct png_data<4,16>
        {
            template<typename Pixel>
            static void assign(std::vector<u8> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                for(size_t i=0; i < list_bytes.size(); i+=8) {
                    // TODO check platform endianess! (currently LE only)
                    list_pixels.emplace_back(Pixel {
                        ((static_cast<u16>(list_bytes[i+1]) << 8) | list_bytes[i+0]),
                        ((static_cast<u16>(list_bytes[i+3]) << 8) | list_bytes[i+2]),
                        ((static_cast<u16>(list_bytes[i+5]) << 8) | list_bytes[i+4]),
                        ((static_cast<u16>(list_bytes[i+7]) << 8) | list_bytes[i+6])
                    });
                }
            }
        };
    }

    // ============================================================= //
    // ============================================================= //

    // Enum values are for the equivalent value in
    // PNG header data
    enum class PNGColorType {
        GREY        = 0, /*greyscale: 1,2,4,8,16 bit*/
        RGB         = 2, /*RGB: 8,16 bit*/
        PALETTE     = 3, /*palette: 1,2,4,8 bit*/
        GREY_ALPHA  = 4, /*greyscale with alpha: 8,16 bit*/
        RGBA        = 6  /*RGB with alpha: 8,16 bit*/
    };

    inline bool LoadPNGFormat(std::vector<u8> const &png_data,
                              PNGColorType &color_type,
                              u8 &bit_depth)
    {
        if(png_data.empty() || png_data.size() < 25) {
            LOG.Error() << "Image: Failed to load png"
                      << ": Invalid header";
            return false;
        }

        // Check the color type and bit depth

        // PNG header:
        // * 8 bytes contain a fixed png id
        // * 4 bytes contain chunk length for the IHDR chunk
        // * 4 bytes contain chunk name for the IHDR chunk
        // * 4 bytes contain width
        // * 4 bytes contain height
        // * 1 byte bit depth  <-- value we want
        // * 1 byte color type <-- value we want
        // ...
        // Multi-byte values are big endian

        bit_depth = png_data[24];
        color_type = static_cast<PNGColorType>(png_data[25]);
        return true;
    }

    inline bool LoadPNGFormat(std::string const &filepath,
                              PNGColorType &color_type,
                              u8 &bitdepth)
    {
        // Load the file into memory
        std::vector<u8> png_data;
        lodepng::load_file(png_data,filepath);
        return LoadPNGFormat(png_data,color_type,bitdepth);
    }

    // ============================================================= //
    // ============================================================= //


    template <typename Pixel>
    bool LoadPNG(std::vector<u8> const &png_data,
                 Image<Pixel> &image,
                 bool * format_match=nullptr)
    {
        // If the source png data colortype and bitdepth
        // match the requested format, lodepng will provide
        // the decoded data in that format

        // Otherwise, lodepng will only convert either to
        // RGB8 or RGBA8 (see lodepng.cpp, lines 4610 and on)

        using namespace image_detail;

        // Default load opts:
        lodepng::State state;
        state.info_raw.bitdepth = 8;
        state.info_raw.colortype = LCT_RGBA;
        bool default_convert = true;

        if(format_match) {
            *format_match = false;
        }

        // Get the source format
        PNGColorType src_colortype = static_cast<PNGColorType>(png_data[25]);
        u8 const src_bitdepth = png_data[24];
        u8 const src_channels =
                (src_colortype == PNGColorType::GREY) ? 1 :
                (src_colortype == PNGColorType::GREY_ALPHA) ? 2 :
                (src_colortype == PNGColorType::RGB) ? 3 :
                (src_colortype == PNGColorType::RGBA) ? 4 : 99; // shouldnt match req_bitdepth

        // Get the requested format
        // cast to u16 in case bitdepth is 64
        constexpr u8 req_bitdepth =
                (!(pixel_traits<Pixel>::is_int_type && pixel_traits<Pixel>::single_bitdepth)) ? 0 :
                (pixel_traits<Pixel>::bits_r > 0) ? pixel_traits<Pixel>::bits_r :
                (pixel_traits<Pixel>::bits_g > 0) ? pixel_traits<Pixel>::bits_g :
                (pixel_traits<Pixel>::bits_b > 0) ? pixel_traits<Pixel>::bits_b :
                (pixel_traits<Pixel>::bits_a > 0) ? pixel_traits<Pixel>::bits_a : 0;

        constexpr u8 req_channels = pixel_traits<Pixel>::channel_count;

        if((src_bitdepth == req_bitdepth) &&
           (src_channels == req_channels))
        {
            // lodepng should decode into the requested
            // format since it matches the png data
            state.info_raw.bitdepth = src_bitdepth;
            state.info_raw.colortype = static_cast<LodePNGColorType>(src_colortype);
            default_convert = false;

            if(format_match) {
                *format_match = true;
            }
        }

        // Decode the png
        unsigned width,height;
        std::vector<u8> list_bytes;
        unsigned error = lodepng::decode(list_bytes,width,height,state,png_data);

        if(error) {
            LOG.Error() << " Image: Failed to load png "
                      << ": " << lodepng_error_text(error);
            return false;
        }
        else {
            unique_ptr<std::vector<Pixel>> list_pixels =
                    make_unique<std::vector<Pixel>>();

            if(!default_convert) {
                list_pixels->reserve(width*height);
                image_detail::png_data<req_channels,req_bitdepth>::assign(list_bytes,*list_pixels);
            }
            else {
                // The source colortype and bitdepth didn't match
                // the corresponding requested params, so we request
                // the data in RGBA8 format and conver to the desired
                // target <Pixel> type
                std::vector<RGBA8> list_temp_pixels;
                list_temp_pixels.reserve(width*height);
                image_detail::png_data<4,8>::assign(list_bytes,list_temp_pixels);
                image_detail::ConvertPixels(list_temp_pixels,*list_pixels);
            }

            // Save data to image
            image.SetAll(width,height,std::move(list_pixels));

        }
        return true;
    }

    template<typename Pixel>
    bool LoadPNG(std::string const &filepath,
                 Image<Pixel> &image,
                 bool * format_match=nullptr)
    {
        std::vector<u8> png_data;
        lodepng::load_file(png_data,filepath);

        if(png_data.size() == 0) {
            LOG.Error() << "Image: Failed to load png file: "
                      << filepath;
            return false;
        }

        return LoadPNG(png_data,image,format_match);
    }

    // ============================================================= //
    // ============================================================= //
}

#endif // KS_IMAGE_PNG_HPP
