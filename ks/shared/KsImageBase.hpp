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

#ifndef KS_IMAGE_BASE_HPP
#define KS_IMAGE_BASE_HPP

// stl
#include <string>
#include <vector>
#include <memory>

// ks
#include <ks/KsLog.hpp>

namespace ks
{
    // ============================================================= //
    // ============================================================= //

    // common pixel types
    struct R8
    {
        u8 r;
    };

    struct R16
    {
        u16 r;
    };

    struct R32
    {
        u32 r;
    };

    struct RGB555 // TODO check memory layout
    {
        u8 r : 5;
        u8 g : 5;
        u8 b : 5;
        u8 : 1; // fill to 16 bits
    };

    struct RGB565 // TODO check memory layout
    {
        u8 r : 5;
        u8 g : 6;
        u8 b : 5;
    };

    struct RGB8
    {
        u8 r;
        u8 g;
        u8 b;
    };

    struct RGBA8
    {
        u8 r;
        u8 g;
        u8 b;
        u8 a;
    };

    struct RGBA16
    {
        u16 r;
        u16 g;
        u16 b;
        u16 a;
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
    // ============================================================= //

    namespace image_detail
    {
        // compile-time int pow // VERIFY its compile time
        constexpr u64 ct_ui_pow(u64 base, u64 exp)
        {
            return ((exp == 0) ? 1 : base*ct_ui_pow(base,exp-1));
        }

        // ============================================================= //

        template <typename Pixel>
        struct pixel_traits
        {
            static const u8 channel_count = 0;

            static const bool is_int_type = false;
            static const bool single_bitdepth = false;

            static const u8 bits_r = 0;
            static const u8 bits_g = 0;
            static const u8 bits_b = 0;
            static const u8 bits_a = 0;
        };

        // ============================================================= //

        // common pixel traits

        template<>
        struct pixel_traits<R8>
        {
            static const u8 channel_count = 1;

            static const bool is_int_type = true;
            static const bool single_bitdepth = true;

            static const u8 bits_r = 8;
            static const u8 bits_g = 0;
            static const u8 bits_b = 0;
            static const u8 bits_a = 0;
        };

        template<>
        struct pixel_traits<R16>
        {
            static const u8 channel_count = 1;

            static const bool is_int_type = true;
            static const bool single_bitdepth = true;

            static const u8 bits_r = 16;
            static const u8 bits_g = 0;
            static const u8 bits_b = 0;
            static const u8 bits_a = 0;
        };

        template<>
        struct pixel_traits<R32>
        {
            static const u8 channel_count = 1;

            static const bool is_int_type = true;
            static const bool single_bitdepth = true;

            static const u8 bits_r = 32;
            static const u8 bits_g = 0;
            static const u8 bits_b = 0;
            static const u8 bits_a = 0;
        };


        template<>
        struct pixel_traits<RGB555>
        {
            static const u8 channel_count = 3;

            static const bool is_int_type = true;
            static const bool single_bitdepth = false;

            static const u8 bits_r = 5;
            static const u8 bits_g = 5;
            static const u8 bits_b = 5;
            static const u8 bits_a = 0;
        };


        template<>
        struct pixel_traits<RGB565>
        {
            static const u8 channel_count = 3;

            static const bool is_int_type = true;
            static const bool single_bitdepth = false;

            static const u8 bits_r = 5;
            static const u8 bits_g = 6;
            static const u8 bits_b = 5;
            static const u8 bits_a = 0;
        };


        template<>
        struct pixel_traits<RGB8>
        {
            static const u8 channel_count = 3;

            static const bool is_int_type = true;
            static const bool single_bitdepth = true;

            static const u8 bits_r = 8;
            static const u8 bits_g = 8;
            static const u8 bits_b = 8;
            static const u8 bits_a = 0;
        };

        template<>
        struct pixel_traits<RGBA8>
        {
            static const u8 channel_count = 4;

            static const bool is_int_type = true;
            static const bool single_bitdepth = true;

            static const u8 bits_r = 8;
            static const u8 bits_g = 8;
            static const u8 bits_b = 8;
            static const u8 bits_a = 8;
        };

        template<>
        struct pixel_traits<RGBA16>
        {
            static const u8 channel_count = 4;

            static const bool is_int_type = true;
            static const bool single_bitdepth = true;

            static const u8 bits_r = 16;
            static const u8 bits_g = 16;
            static const u8 bits_b = 16;
            static const u8 bits_a = 16;
        };

        template<>
        struct pixel_traits<RGB32F>
        {
            static const u8 channel_count = 3;

            static const bool is_int_type = false;
            static const bool single_bitdepth = true;

            static const u8 bits_r = 32;
            static const u8 bits_g = 32;
            static const u8 bits_b = 32;
            static const u8 bits_a = 0;
        };

        template<>
        struct pixel_traits<RGBA32F>
        {
            static const u8 channel_count = 4;

            static const bool is_int_type = false;
            static const bool single_bitdepth = true;

            static const u8 bits_r = 32;
            static const u8 bits_g = 32;
            static const u8 bits_b = 32;
            static const u8 bits_a = 32;
        };

        template<>
        struct pixel_traits<RGBA64F>
        {
            static const u8 channel_count = 4;

            static const bool is_int_type = false;
            static const bool single_bitdepth = true;

            static const u8 bits_r = 64;
            static const u8 bits_g = 64;
            static const u8 bits_b = 64;
            static const u8 bits_a = 64;
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
                constexpr u8 ds_shift = pixel_traits<PixelSrc>::bits_r-pixel_traits<PixelDst>::bits_r;
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
                constexpr u8 ds_shift = pixel_traits<PixelSrc>::bits_g-pixel_traits<PixelDst>::bits_g;
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
                constexpr u8 ds_shift = pixel_traits<PixelSrc>::bits_b-pixel_traits<PixelDst>::bits_b;
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
                constexpr u8 ds_shift = pixel_traits<PixelSrc>::bits_a-pixel_traits<PixelDst>::bits_a;
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
        void ConvertPixels(std::vector<PixelSrc> const &list_src,
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

    } // detail

    // ============================================================= //
    // ============================================================= //

    struct PixelTraits
    {
        u8 channel_count;

        bool is_int_type;

        // ie. RGB888 --> true since all channels are 8 bits
        //     RGB565 --> false since channels have diff bitdepths
        bool single_bitdepth;

        u8 bits_r;
        u8 bits_g;
        u8 bits_b;
        u8 bits_a;
    };

    // type erasure convenience struct
    struct ImageData
    {
        ImageData() :
            // can't initialize unique_ptr<void> w/o deleter
            width(0),
            height(0),
            list_pixels(nullptr,[](void*){}),
            data_ptr(nullptr)
        {}

        uint width;
        uint height;
        PixelTraits pixel_traits;

        // <Image.data, Image.data.deleter>
        // This points to vector<Pixel>
        std::unique_ptr<void,void(*)(void*)> list_pixels;

        // This points to &(vector<Pixel>[0])
        void const * data_ptr;
    };

    // ============================================================= //
    // ============================================================= //

    template<typename Pixel>
    class Image final
    {
    public:
        using PixelIterator = typename std::vector<Pixel>::iterator;
        using ConstPixelIterator = typename std::vector<Pixel>::const_iterator;

        Image() :
            m_width(0),
            m_height(0)
        {
            m_data = make_unique<std::vector<Pixel>>();
        }

        Image(uint width, uint height) :
            m_width(width),
            m_height(height)
        {
            m_data = make_unique<std::vector<Pixel>>();
        }

        Image(uint width, uint height, Pixel fill) :
            m_width(width),
            m_height(height)
        {
            m_data = make_unique<std::vector<Pixel>>(width*height,fill);
        }

        ~Image() = default;

        uint GetWidth() const
        {
            return m_width;
        }

        uint GetHeight() const
        {
            return m_height;
        }

        PixelTraits GetPixelTraits() const
        {
            PixelTraits traits;
            traits.channel_count = image_detail::pixel_traits<Pixel>::channel_count;
            traits.is_int_type = image_detail::pixel_traits<Pixel>::is_int_type;
            traits.single_bitdepth = image_detail::pixel_traits<Pixel>::single_bitdepth;
            traits.bits_r = image_detail::pixel_traits<Pixel>::bits_r;
            traits.bits_g = image_detail::pixel_traits<Pixel>::bits_g;
            traits.bits_b = image_detail::pixel_traits<Pixel>::bits_b;
            traits.bits_a = image_detail::pixel_traits<Pixel>::bits_a;

            return traits;
        }

        ConstPixelIterator GetPixel(uint col, uint row) const
        {
            auto it = (*m_data).begin();
            std::advance(it,(row*m_width)+col);
            return it;
        }

        PixelIterator GetPixel(uint col, uint row)
        {
            auto it = (*m_data).begin();
            std::advance(it,(row*m_width)+col);
            return it;
        }

        std::vector<Pixel> const & GetData() const
        {
            return (*m_data);
        }

        std::vector<Pixel>& GetData()
        {
            return (*m_data);
        }

        void SetAll(uint width, uint height,
                    std::vector<Pixel> const &data)
        {
            m_width = width;
            m_height = height;
            (*m_data) = data;
        }

        void SetAll(uint width, uint height,
                    unique_ptr<std::vector<Pixel>> data)
        {
            m_width = width;
            m_height = height;
            m_data = std::move(data);
        }

        void Insert(Image<Pixel> const &source,
                    PixelIterator source_it,
                    PixelIterator target_it)
        {
            Insert(source,
                   source_it,
                   source.GetWidth(),
                   source.GetHeight(),
                   target_it);
        }

        void Insert(Image<Pixel> const &source,
                    PixelIterator source_it,
                    uint source_cols,
                    uint source_rows,
                    PixelIterator target_it)
        {
            Image<Pixel> &target = (*this);

            u32 const overlap_rows =
                    std::min(target.GetHeight()-target.row(target_it),
                             source_rows);

            u32 const overlap_cols =
                    std::min(target.GetWidth()-target.col(target_it),
                             source_cols);

            // row by row
            for(u32 i=0; i < overlap_rows; i++) {
                auto source_row_end = source_it;
                std::advance(source_row_end,overlap_cols);
                std::copy(source_it,source_row_end,target_it);

                // advance source and target its by a row
                std::advance(source_it,source.GetWidth());
                std::advance(target_it,target.GetWidth());
            }
        }

        // Moves all data outside of this image, leaving
        // it in an invalid state
        ImageData ConvertToImageData()
        {
            ImageData im_data;
            im_data.width = m_width;
            im_data.height = m_height;
            im_data.pixel_traits = this->GetPixelTraits();
            im_data.list_pixels.reset(m_data.release());
            im_data.list_pixels.get_deleter() =
                    [](void * data) {
                        std::vector<Pixel> * p = static_cast<std::vector<Pixel>*>(data);
                        delete p;
                    };

            std::vector<Pixel> * p =
                    static_cast<std::vector<Pixel>*>(
                        im_data.list_pixels.get());

            im_data.data_ptr = &((*p)[0]);

            return im_data;
        }

        unique_ptr<ImageData> ConvertToImageDataPtr()
        {
            unique_ptr<ImageData> im_data_ptr =
                    make_unique<ImageData>(this->ConvertToImageData());

            return im_data_ptr;
        }


    private:
        uint col(PixelIterator it) const
        {
            return std::distance((*m_data).begin(),it)%m_width;
        }

        uint row(PixelIterator it) const
        {
            return std::distance((*m_data).begin(),it)/m_width;
        }


        uint m_width;
        uint m_height;
        unique_ptr<std::vector<Pixel>> m_data;
    };

    // ============================================================= //
    // ============================================================= //
}

#endif // KS_IMAGE_BASE_HPP
