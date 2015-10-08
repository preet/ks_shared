/*
   Copyright (C) 2015 Preet Desai (preet.desai@gmail.com)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef KS_IMAGE_HPP
#define KS_IMAGE_HPP

// sys
#include <cstdint>

// stl
#include <string>
#include <vector>
#include <memory>

// lodepng
#include <lodepng/lodepng.h>

// ks
#include <ks/KsLog.hpp>

namespace ks
{
    // NOTE:
    // implementation is from scratch/utils/inlineimage

    // common pixel types
    struct R8
    {
        uint8_t r;
    };

    struct R16
    {
        uint16_t r;
    };

    struct R32
    {
        uint32_t r;
    };

    struct RGB555 // TODO check memory layout
    {
        uint8_t r : 5;
        uint8_t g : 5;
        uint8_t b : 5;
        uint8_t : 1; // fill to 16 bits
    };

    struct RGB565 // TODO check memory layout
    {
        uint8_t r : 5;
        uint8_t g : 6;
        uint8_t b : 5;
    };

    struct RGB8
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    struct RGBA8
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    struct RGBA16
    {
        uint16_t r;
        uint16_t g;
        uint16_t b;
        uint16_t a;
    };

    struct RGB32F
    {
        float r;
        float g;
        float b;
    };

    struct RGBA32F
    {
        float r;
        float g;
        float b;
        float a;
    };

    struct RGBA64F
    {
        double r;
        double g;
        double b;
        double a;
    };

    // ============================================================= //

    namespace ilim_detail
    {
        // compile-time int pow // VERIFY its compile time
        constexpr uint64_t ct_ui_pow(uint64_t base, uint64_t exp)
        {
            return ((exp == 0) ? 1 : base*ct_ui_pow(base,exp-1));
        }

        // ============================================================= //

        template <typename Pixel>
        struct pixel_traits
        {
            static const uint8_t channel_count = 0;

            static const bool is_int_type = false;
            static const bool single_bitdepth = false;

            static const uint8_t bits_r = 0;
            static const uint8_t bits_g = 0;
            static const uint8_t bits_b = 0;
            static const uint8_t bits_a = 0;
        };

        // ============================================================= //

        // common pixel traits

        template<>
        struct pixel_traits<R8>
        {
            static const uint8_t channel_count = 1;

            static const bool is_int_type = true;
            static const bool single_bitdepth = true;

            static const uint8_t bits_r = 8;
            static const uint8_t bits_g = 0;
            static const uint8_t bits_b = 0;
            static const uint8_t bits_a = 0;
        };

        template<>
        struct pixel_traits<R16>
        {
            static const uint8_t channel_count = 1;

            static const bool is_int_type = true;
            static const bool single_bitdepth = true;

            static const uint8_t bits_r = 16;
            static const uint8_t bits_g = 0;
            static const uint8_t bits_b = 0;
            static const uint8_t bits_a = 0;
        };

        template<>
        struct pixel_traits<R32>
        {
            static const uint8_t channel_count = 1;

            static const bool is_int_type = true;
            static const bool single_bitdepth = true;

            static const uint8_t bits_r = 32;
            static const uint8_t bits_g = 0;
            static const uint8_t bits_b = 0;
            static const uint8_t bits_a = 0;
        };


        template<>
        struct pixel_traits<RGB555>
        {
            static const uint8_t channel_count = 3;

            static const bool is_int_type = true;
            static const bool single_bitdepth = false;

            static const uint8_t bits_r = 5;
            static const uint8_t bits_g = 5;
            static const uint8_t bits_b = 5;
            static const uint8_t bits_a = 0;
        };


        template<>
        struct pixel_traits<RGB565>
        {
            static const uint8_t channel_count = 3;

            static const bool is_int_type = true;
            static const bool single_bitdepth = false;

            static const uint8_t bits_r = 5;
            static const uint8_t bits_g = 6;
            static const uint8_t bits_b = 5;
            static const uint8_t bits_a = 0;
        };


        template<>
        struct pixel_traits<RGB8>
        {
            static const uint8_t channel_count = 3;

            static const bool is_int_type = true;
            static const bool single_bitdepth = true;

            static const uint8_t bits_r = 8;
            static const uint8_t bits_g = 8;
            static const uint8_t bits_b = 8;
            static const uint8_t bits_a = 0;
        };

        template<>
        struct pixel_traits<RGBA8>
        {
            static const uint8_t channel_count = 4;

            static const bool is_int_type = true;
            static const bool single_bitdepth = true;

            static const uint8_t bits_r = 8;
            static const uint8_t bits_g = 8;
            static const uint8_t bits_b = 8;
            static const uint8_t bits_a = 8;
        };

        template<>
        struct pixel_traits<RGBA16>
        {
            static const uint8_t channel_count = 4;

            static const bool is_int_type = true;
            static const bool single_bitdepth = true;

            static const uint8_t bits_r = 16;
            static const uint8_t bits_g = 16;
            static const uint8_t bits_b = 16;
            static const uint8_t bits_a = 16;
        };

        template<>
        struct pixel_traits<RGB32F>
        {
            static const uint8_t channel_count = 3;

            static const bool is_int_type = false;
            static const bool single_bitdepth = true;

            static const uint8_t bits_r = 32;
            static const uint8_t bits_g = 32;
            static const uint8_t bits_b = 32;
            static const uint8_t bits_a = 0;
        };

        template<>
        struct pixel_traits<RGBA32F>
        {
            static const uint8_t channel_count = 4;

            static const bool is_int_type = false;
            static const bool single_bitdepth = true;

            static const uint8_t bits_r = 32;
            static const uint8_t bits_g = 32;
            static const uint8_t bits_b = 32;
            static const uint8_t bits_a = 32;
        };

        template<>
        struct pixel_traits<RGBA64F>
        {
            static const uint8_t channel_count = 4;

            static const bool is_int_type = false;
            static const bool single_bitdepth = true;

            static const uint8_t bits_r = 64;
            static const uint8_t bits_g = 64;
            static const uint8_t bits_b = 64;
            static const uint8_t bits_a = 64;
        };

        // ============================================================= //

        enum class assign_mode {
            no_op,
            sub,
            int_to_float,
            float_to_int,
            float_to_float,
            upscale,
            downscale
        };

        // ============================================================= //

        template<assign_mode mode>
        struct channel_r;

        // no-op: dst channel dne
        template<>
        struct channel_r<assign_mode::no_op>
        {
            template<typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &, PixelDst &)
            {
                // no op
            }
        };

        template<>
        struct channel_r<assign_mode::sub> {
            template<typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &, PixelDst &dst) {
                dst.r = 0;
            }
        };

        template<>
        struct channel_r<assign_mode::int_to_float> {
            template <typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                constexpr decltype(PixelDst::r) max = ct_ui_pow(2,pixel_traits<PixelSrc>::bits_r)-1;
                dst.r = src.r/max;
            }
        };

        template<>
        struct channel_r<assign_mode::float_to_int> {
            template <typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                constexpr auto max = ct_ui_pow(2,pixel_traits<PixelDst>::bits_r)-1;
                dst.r = src.r*max;
            }
        };

        template<>
        struct channel_r<assign_mode::float_to_float> {
            template <typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                dst.r = src.r;
            }
        };

        template<>
        struct channel_r<assign_mode::upscale> {
            template<typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                constexpr decltype(PixelSrc::r) us_div = ct_ui_pow(2,pixel_traits<PixelSrc>::bits_r)-1;
                constexpr decltype(PixelDst::r) us_mul = ct_ui_pow(2,pixel_traits<PixelDst>::bits_r)-1;
                dst.r = (us_mul/us_div)*src.r;
            }
        };

        template<>
        struct channel_r<assign_mode::downscale> {
            template<typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                constexpr uint8_t ds_shift = pixel_traits<PixelSrc>::bits_r-pixel_traits<PixelDst>::bits_r;
                dst.r = (src.r >> ds_shift);
            }
        };

        template<typename PixelSrc,typename PixelDst>
        void assign_r(PixelSrc const &src, PixelDst &dst)
        {
            typedef pixel_traits<PixelSrc> src_traits;
            typedef pixel_traits<PixelDst> dst_traits;

            constexpr assign_mode mode = (
                        (dst_traits::bits_r==0) ? assign_mode::no_op :
                        (src_traits::bits_r==0) ? assign_mode::sub :
                        (src_traits::is_int_type && !dst_traits::is_int_type) ? assign_mode::int_to_float :
                        (!src_traits::is_int_type && dst_traits::is_int_type) ? assign_mode::float_to_int :
                        (!src_traits::is_int_type && !dst_traits::is_int_type) ? assign_mode::float_to_float :
                        (dst_traits::bits_r > src_traits::bits_r) ? assign_mode::upscale : assign_mode::downscale);

            channel_r<mode>::assign(src,dst);
        }

        // ============================================================= //

        template<assign_mode mode>
        struct channel_g;

        // no-op: dst channel dne
        template<>
        struct channel_g<assign_mode::no_op>
        {
            template<typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &, PixelDst &)
            {
                // no op
            }
        };

        template<>
        struct channel_g<assign_mode::sub> {
            template<typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &, PixelDst &dst) {
                dst.g = 0;
            }
        };

        template<>
        struct channel_g<assign_mode::int_to_float> {
            template <typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                constexpr decltype(PixelDst::g) max = ct_ui_pow(2,pixel_traits<PixelSrc>::bits_g)-1;
                dst.g = src.g/max;
            }
        };

        template<>
        struct channel_g<assign_mode::float_to_int> {
            template <typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                constexpr auto max = ct_ui_pow(2,pixel_traits<PixelDst>::bits_g)-1;
                dst.g = src.g*max;
            }
        };

        template<>
        struct channel_g<assign_mode::float_to_float> {
            template <typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                dst.g = src.g;
            }
        };

        template<>
        struct channel_g<assign_mode::upscale> {
            template<typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                constexpr decltype(PixelSrc::g) us_div = ct_ui_pow(2,pixel_traits<PixelSrc>::bits_g)-1;
                constexpr decltype(PixelDst::g) us_mul = ct_ui_pow(2,pixel_traits<PixelDst>::bits_g)-1;
                dst.g = (us_mul/us_div)*src.g;
            }
        };

        template<>
        struct channel_g<assign_mode::downscale> {
            template<typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                constexpr uint8_t ds_shift = pixel_traits<PixelSrc>::bits_g-pixel_traits<PixelDst>::bits_g;
                dst.g = (src.g >> ds_shift);
            }
        };

        template<typename PixelSrc,typename PixelDst>
        void assign_g(PixelSrc const &src, PixelDst &dst)
        {
            typedef pixel_traits<PixelSrc> src_traits;
            typedef pixel_traits<PixelDst> dst_traits;

            constexpr assign_mode mode = (
                        (dst_traits::bits_g==0) ? assign_mode::no_op :
                        (src_traits::bits_g==0) ? assign_mode::sub :
                        (src_traits::is_int_type && !dst_traits::is_int_type) ? assign_mode::int_to_float :
                        (!src_traits::is_int_type && dst_traits::is_int_type) ? assign_mode::float_to_int :
                        (!src_traits::is_int_type && !dst_traits::is_int_type) ? assign_mode::float_to_float :
                        (dst_traits::bits_g > src_traits::bits_g) ? assign_mode::upscale : assign_mode::downscale);

            channel_g<mode>::assign(src,dst);
        }

        // ============================================================= //

        template<assign_mode mode>
        struct channel_b;

        // no-op: dst channel dne
        template<>
        struct channel_b<assign_mode::no_op>
        {
            template<typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &, PixelDst &)
            {
                // no op
            }
        };

        template<>
        struct channel_b<assign_mode::sub> {
            template<typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &, PixelDst &dst) {
                dst.b = 0;
            }
        };

        template<>
        struct channel_b<assign_mode::int_to_float> {
            template <typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                constexpr decltype(PixelDst::b) max = ct_ui_pow(2,pixel_traits<PixelSrc>::bits_b)-1;
                dst.b = src.b/max;
            }
        };

        template<>
        struct channel_b<assign_mode::float_to_int> {
            template <typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                constexpr auto max = ct_ui_pow(2,pixel_traits<PixelDst>::bits_b)-1;
                dst.b = src.b*max;
            }
        };

        template<>
        struct channel_b<assign_mode::float_to_float> {
            template <typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                dst.b = src.b;
            }
        };

        template<>
        struct channel_b<assign_mode::upscale> {
            template<typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                constexpr decltype(PixelSrc::b) us_div = ct_ui_pow(2,pixel_traits<PixelSrc>::bits_b)-1;
                constexpr decltype(PixelDst::b) us_mul = ct_ui_pow(2,pixel_traits<PixelDst>::bits_b)-1;
                dst.b = (us_mul/us_div)*src.b;
            }
        };

        template<>
        struct channel_b<assign_mode::downscale> {
            template<typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                constexpr uint8_t ds_shift = pixel_traits<PixelSrc>::bits_b-pixel_traits<PixelDst>::bits_b;
                dst.b = (src.b >> ds_shift);
            }
        };

        template<typename PixelSrc,typename PixelDst>
        void assign_b(PixelSrc const &src, PixelDst &dst)
        {
            typedef pixel_traits<PixelSrc> src_traits;
            typedef pixel_traits<PixelDst> dst_traits;

            constexpr assign_mode mode = (
                        (dst_traits::bits_b==0) ? assign_mode::no_op :
                        (src_traits::bits_b==0) ? assign_mode::sub :
                        (src_traits::is_int_type && !dst_traits::is_int_type) ? assign_mode::int_to_float :
                        (!src_traits::is_int_type && dst_traits::is_int_type) ? assign_mode::float_to_int :
                        (!src_traits::is_int_type && !dst_traits::is_int_type) ? assign_mode::float_to_float :
                        (dst_traits::bits_b > src_traits::bits_b) ? assign_mode::upscale : assign_mode::downscale);

            channel_b<mode>::assign(src,dst);
        }

        // ============================================================= //

        template<assign_mode mode>
        struct channel_a;

        // no-op: dst channel dne
        template<>
        struct channel_a<assign_mode::no_op>
        {
            template<typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &, PixelDst &)
            {
                // no op
            }
        };

        template<>
        struct channel_a<assign_mode::sub> {
            template<typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &, PixelDst &dst) {
                dst.a = 1;
            }
        };

        template<>
        struct channel_a<assign_mode::int_to_float> {
            template <typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                constexpr decltype(PixelDst::a) max = ct_ui_pow(2,pixel_traits<PixelSrc>::bits_a)-1;
                dst.a = src.a/max;
            }
        };

        template<>
        struct channel_a<assign_mode::float_to_int> {
            template <typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                constexpr auto max = ct_ui_pow(2,pixel_traits<PixelDst>::bits_a)-1;
                dst.a = src.a*max;
            }
        };

        template<>
        struct channel_a<assign_mode::float_to_float> {
            template <typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                dst.a = src.a;
            }
        };

        template<>
        struct channel_a<assign_mode::upscale> {
            template<typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                constexpr decltype(PixelSrc::a) us_div = ct_ui_pow(2,pixel_traits<PixelSrc>::bits_a)-1;
                constexpr decltype(PixelDst::a) us_mul = ct_ui_pow(2,pixel_traits<PixelDst>::bits_a)-1;
                dst.a = (us_mul/us_div)*src.a;
            }
        };

        template<>
        struct channel_a<assign_mode::downscale> {
            template<typename PixelSrc, typename PixelDst>
            static void assign(PixelSrc const &src, PixelDst &dst) {
                constexpr uint8_t ds_shift = pixel_traits<PixelSrc>::bits_a-pixel_traits<PixelDst>::bits_a;
                dst.a = (src.a >> ds_shift);
            }
        };

        template<typename PixelSrc,typename PixelDst>
        void assign_a(PixelSrc const &src, PixelDst &dst)
        {
            typedef pixel_traits<PixelSrc> src_traits;
            typedef pixel_traits<PixelDst> dst_traits;

            constexpr assign_mode mode = (
                        (dst_traits::bits_a==0) ? assign_mode::no_op :
                        (src_traits::bits_a==0) ? assign_mode::sub :
                        (src_traits::is_int_type && !dst_traits::is_int_type) ? assign_mode::int_to_float :
                        (!src_traits::is_int_type && dst_traits::is_int_type) ? assign_mode::float_to_int :
                        (!src_traits::is_int_type && !dst_traits::is_int_type) ? assign_mode::float_to_float :
                        (dst_traits::bits_a > src_traits::bits_a) ? assign_mode::upscale : assign_mode::downscale);

            channel_a<mode>::assign(src,dst);
        }

        // ============================================================= //

        template <typename PixelSrc,typename PixelDst>
        void conv_pixels(std::vector<PixelSrc> const &list_src,
                         std::vector<PixelDst> &list_dst)
        {
            list_dst.clear();
            list_dst.reserve(list_src.size());

            // int -> int
            for(auto const &src : list_src) {
                PixelDst dst;
                assign_r(src,dst);
                assign_g(src,dst);
                assign_b(src,dst);
                assign_a(src,dst);

                list_dst.push_back(dst);
            }
        }

        // ref: http://stackoverflow.com/questions/105252/...
        // ...how-do-i-convert-between-big-endian-and-little-endian-values-in-c/105297#105297
        template <typename T>
        T swap_endian(T u)
        {
            union
            {
                T u;
                unsigned char u8[sizeof(T)];
            } source, dest;

            source.u = u;

            for (size_t k = 0; k < sizeof(T); k++)
                dest.u8[k] = source.u8[sizeof(T) - k - 1];

            return dest.u;
        }

    } // detail

    // ============================================================= //
    // ============================================================= //

    //
    struct PixelTraits
    {
        uint8_t channel_count;

        bool is_int_type;
        bool single_bitdepth;

        uint8_t bits_r;
        uint8_t bits_g;
        uint8_t bits_b;
        uint8_t bits_a;
    };

    // type erasure convenience struct
    struct ImageData
    {
        ImageData() :
            data(nullptr,[](void*){}), // can't initialize unique_ptr<void> w/o deleter
            data_ptr(nullptr)
        {
            // empty
        }

        uint32_t width;
        uint32_t height;
        PixelTraits pixel_traits;

        // <Image.data, Image.data.deleter>
        std::unique_ptr<void,void(*)(void*)> data;

        void const * data_ptr;
    };

    // ============================================================= //
    // ============================================================= //

    template<typename Pixel>
    class Image
    {
    public:
        typedef typename std::vector<Pixel>::iterator PixelIterator;

    private:
        uint32_t m_width;
        uint32_t m_height;
        std::unique_ptr<std::vector<Pixel>> m_data;


        uint32_t col(PixelIterator it)
        {
            return std::distance((*m_data).begin(),it)%m_width;
        }

        uint32_t row(PixelIterator it)
        {
            return std::distance((*m_data).begin(),it)/m_width;
        }

    public:
        // initialize null image
        Image() :
            m_width(0),
            m_height(0),
            m_data(new std::vector<Pixel>)
        {
            // empty
        }

        // leaves this Image in an invalid state
        ImageData conv_to_image_data()
        {
            ImageData im_data;
            im_data.width = m_width;
            im_data.height = m_height;
            im_data.data.reset(m_data.release());
            im_data.data.get_deleter() = [](void * data) {
                std::vector<Pixel> * p = static_cast<std::vector<Pixel>*>(data);
                delete p;
            };

            std::vector<Pixel> * p =
                    static_cast<std::vector<Pixel>*>(
                        im_data.data.get());

            im_data.data_ptr = &((*p)[0]);

            return std::move(im_data);
        }

        void set(ImageData & image_data)
        {
            m_width  = image_data.width;
            m_height = image_data.height;
            m_data.reset(static_cast<std::vector<Pixel>*>(
                             image_data.data.release()));
        }

        // set; rename assign?
        void set(uint32_t width,
                 uint32_t height,
                 std::vector<Pixel> && data)
        {
            m_width = width;
            m_height = height;
            (*m_data) = std::move(data);
        }

        void set(uint32_t width,
                 uint32_t height,
                 std::vector<Pixel> const &data)
        {
            m_width = width;
            m_height = height;
            (*m_data) = data;
        }

        uint32_t width() const
        {
            return m_width;
        }

        uint32_t height() const
        {
            return m_height;
        }

        PixelTraits pixel_traits() const
        {
            PixelTraits traits;
            traits.channel_count = ilim_detail::pixel_traits<Pixel>::channel_count;
            traits.is_int_type = ilim_detail::pixel_traits<Pixel>::is_int_type;
            traits.single_bitdepth = ilim_detail::pixel_traits<Pixel>::single_bitdepth;
            traits.bits_r = ilim_detail::pixel_traits<Pixel>::bits_r;
            traits.bits_g = ilim_detail::pixel_traits<Pixel>::bits_g;
            traits.bits_b = ilim_detail::pixel_traits<Pixel>::bits_b;
            traits.bits_a = ilim_detail::pixel_traits<Pixel>::bits_a;

            return traits;
        }

        PixelIterator at(uint32_t col, uint32_t row)
        {
            auto it = (*m_data).begin();
            std::advance(it,(row*m_width)+col);
            return it;
        }

        PixelIterator end()
        {
            return (*m_data).end();
        }

        PixelIterator begin()
        {
            return (*m_data).begin();
        }

        PixelIterator last()
        {
            auto it_last = (*m_data).end();
            std::advance(it_last,-1);
            return it_last;
        }

        std::vector<Pixel>& data()
        {
            return (*m_data);
        }

        //
        void insert(Image<Pixel> const &source,
                    PixelIterator source_it,
                    PixelIterator target_it)
        {
            insert(source,
                   source_it,
                   source.width(),
                   source.height(),
                   target_it);
        }

        void insert(Image<Pixel> const &source,
                    PixelIterator source_it,
                    uint32_t source_cols,
                    uint32_t source_rows,
                    PixelIterator target_it)
        {
            Image<Pixel> &target = (*this);

            uint32_t const overlap_rows =
                    std::min(target.height()-target.row(target_it),
                             source_rows);

            uint32_t const overlap_cols =
                    std::min(target.width()-target.col(target_it),
                             source_cols);

            // row by row
            for(uint32_t i=0; i < overlap_rows; i++) {
                auto source_row_end = source_it;
                std::advance(source_row_end,overlap_cols);
                std::copy(source_it,source_row_end,target_it);

                // advance source and target its by a row
                std::advance(source_it,source.width());
                std::advance(target_it,target.width());
            }
        }
    };

    // ============================================================= //
    // ============================================================= //

    // ilim_png
    // enum values are for the equivalent value in
    // PNG header data
    enum class PNGColorType {
        GREY        = 0, /*greyscale: 1,2,4,8,16 bit*/
        RGB         = 2, /*RGB: 8,16 bit*/
        PALETTE     = 3, /*palette: 1,2,4,8 bit*/
        GREY_ALPHA  = 4, /*greyscale with alpha: 8,16 bit*/
        RGBA        = 6  /*RGB with alpha: 8,16 bit*/
    };

    inline bool load_png_format(std::vector<uint8_t> const &png_data,
                                PNGColorType &color_type,
                                uint8_t &bit_depth)
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

    inline bool load_png_format(std::string const &filepath,
                                PNGColorType &color_type,
                                uint8_t &bitdepth)
    {
        // Load the file into memory
        std::vector<uint8_t> png_data;
        lodepng::load_file(png_data,filepath);
        return load_png_format(png_data,color_type,bitdepth);
    }

    namespace ilim_detail
    {
        template<uint8_t Channels, uint8_t BitDepth>
        struct png_data
        {
            template<typename Pixel>
            static void assign(std::vector<uint8_t> const &,
                               std::vector<Pixel> &)
            {
                // do nothing
            }
        };

        template<>
        struct png_data<1,1>
        {
            template<typename Pixel>
            static void assign(std::vector<uint8_t> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                for(uint8_t const byte : list_bytes) {
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
            static void assign(std::vector<uint8_t> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                for(uint8_t const byte : list_bytes) {
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
            static void assign(std::vector<uint8_t> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                for(uint8_t const byte : list_bytes) {
                    list_pixels.emplace_back(Pixel{ byte & 15 });
                    list_pixels.emplace_back(Pixel{ byte & 240 });
                }
            }
        };

        template<>
        struct png_data<1,8>
        {
            template<typename Pixel>
            static void assign(std::vector<uint8_t> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                for(uint8_t const byte : list_bytes) {
                    list_pixels.emplace_back(Pixel{ byte });
                }
            }
        };

        template<>
        struct png_data<1,16>
        {
            template<typename Pixel>
            static void assign(std::vector<uint8_t> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                // png data is in big endian
                for(size_t i=0; i < list_bytes.size(); i+=2) {
                    // TODO check platform endianess! (currently LE only)
                    list_pixels.emplace_back(Pixel {
                        (static_cast<uint16_t>(list_bytes[i+1]) << 8) | list_bytes[i]
                    });
                }
            }
        };

        template<>
        struct png_data<2,8>
        {
            template<typename Pixel>
            static void assign(std::vector<uint8_t> const &list_bytes,
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
            static void assign(std::vector<uint8_t> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                for(size_t i=0; i < list_bytes.size(); i+=4) {
                    // TODO check platform endianess! (currently LE only)
                    list_pixels.emplace_back(Pixel {
                        ((static_cast<uint16_t>(list_bytes[i+1]) << 8) | list_bytes[i+0]),
                        ((static_cast<uint16_t>(list_bytes[i+3]) << 8) | list_bytes[i+2])
                    });
                }
            }
        };

        template<>
        struct png_data<3,8>
        {
            template<typename Pixel>
            static void assign(std::vector<uint8_t> const &list_bytes,
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
            static void assign(std::vector<uint8_t> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                for(size_t i=0; i < list_bytes.size(); i+=6) {
                    // TODO check platform endianess! (currently LE only)
                    list_pixels.emplace_back(Pixel {
                        ((static_cast<uint16_t>(list_bytes[i+1]) << 8) | list_bytes[i+0]),
                        ((static_cast<uint16_t>(list_bytes[i+3]) << 8) | list_bytes[i+2]),
                        ((static_cast<uint16_t>(list_bytes[i+5]) << 8) | list_bytes[i+4])
                    });
                }
            }
        };

        template<>
        struct png_data<4,8>
        {
            template<typename Pixel>
            static void assign(std::vector<uint8_t> const &list_bytes,
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
            static void assign(std::vector<uint8_t> const &list_bytes,
                               std::vector<Pixel> &list_pixels)
            {
                for(size_t i=0; i < list_bytes.size(); i+=8) {
                    // TODO check platform endianess! (currently LE only)
                    list_pixels.emplace_back(Pixel {
                        ((static_cast<uint16_t>(list_bytes[i+1]) << 8) | list_bytes[i+0]),
                        ((static_cast<uint16_t>(list_bytes[i+3]) << 8) | list_bytes[i+2]),
                        ((static_cast<uint16_t>(list_bytes[i+5]) << 8) | list_bytes[i+4]),
                        ((static_cast<uint16_t>(list_bytes[i+7]) << 8) | list_bytes[i+6])
                    });
                }
            }
        };
    }

    template <typename Pixel>
    bool load_png(std::vector<uint8_t> const &png_data,
                  Image<Pixel> &image,
                  bool * format_match=nullptr)
    {
        // If the source png data colortype and bitdepth
        // match the requested format, lodepng will provide
        // the decoded data in that format

        // Otherwise, lodepng will only convert either to
        // RGB8 or RGBA8 (see lodepng.cpp, lines 4610 and on)

        using namespace ilim_detail;

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
        uint8_t const src_bitdepth = png_data[24];
        uint8_t const src_channels =
                (src_colortype == PNGColorType::GREY) ? 1 :
                (src_colortype == PNGColorType::GREY_ALPHA) ? 2 :
                (src_colortype == PNGColorType::RGB) ? 3 :
                (src_colortype == PNGColorType::RGBA) ? 4 : 99; // shouldnt match req_bitdepth

        // Get the requested format
        // cast to uint16_t in case bitdepth is 64
        constexpr uint8_t req_bitdepth =
                (!(pixel_traits<Pixel>::is_int_type && pixel_traits<Pixel>::single_bitdepth)) ? 0 :
                (pixel_traits<Pixel>::bits_r > 0) ? pixel_traits<Pixel>::bits_r :
                (pixel_traits<Pixel>::bits_g > 0) ? pixel_traits<Pixel>::bits_g :
                (pixel_traits<Pixel>::bits_b > 0) ? pixel_traits<Pixel>::bits_b :
                (pixel_traits<Pixel>::bits_a > 0) ? pixel_traits<Pixel>::bits_a : 0;

        constexpr uint8_t req_channels = pixel_traits<Pixel>::channel_count;

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
        std::vector<uint8_t> list_bytes;
        unsigned error = lodepng::decode(list_bytes,width,height,state,png_data);

        if(error) {
            LOG.Error() << " Image: Failed to load png "
                      << ": " << lodepng_error_text(error);
            return false;
        }
        else {
            std::vector<Pixel> list_pixels;

            if(!default_convert) {
                list_pixels.reserve(width*height);
                ilim_detail::png_data<req_channels,req_bitdepth>::assign(list_bytes,list_pixels);
            }
            else {
                // The source colortype and bitdepth didn't match
                // the corresponding requested params, so we request
                // the data in RGBA8 format and conver to the desired
                // target <Pixel> type
                std::vector<RGBA8> list_temp_pixels;
                list_temp_pixels.reserve(width*height);
                ilim_detail::png_data<4,8>::assign(list_bytes,list_temp_pixels);
                conv_pixels(list_temp_pixels,list_pixels);
            }

            // Save data to image
            image.set(width,height,std::move(list_pixels));

        }
        return true;
    }

    template <typename Pixel>
    bool load_png(std::string const &filepath,
                  Image<Pixel> &image,
                  bool * format_match=nullptr)
    {
        std::vector<uint8_t> png_data;
        lodepng::load_file(png_data,filepath);

        if(png_data.size() == 0) {
            LOG.Error() << "Image: Failed to load png file: "
                      << filepath;
            return false;
        }

        return load_png(png_data,image,format_match);
    }

    // ============================================================= //

} // ks


#endif // KS_IMAGE_HPP
