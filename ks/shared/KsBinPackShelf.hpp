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

#ifndef KS_BINPACK_SHELF_HPP
#define KS_BINPACK_SHELF_HPP

#include <vector>
#include <ks/KsGlobal.hpp>

namespace ks
{
    class BinPackRectangle
    {
    public:
        uint x;     // left
        uint y;     // top
        uint width;
        uint height;
        bool flipped;

        BinPackRectangle() :
            x(0),
            y(0),
            width(0),
            height(0),
            flipped(false)
        {}
    };

    class BinPackShelf
    {
    public:
        BinPackShelf(uint width,
                     uint height,
                     uint spacing);

        uint GetWidth() const;
        uint GetHeight() const;

        // AddRectangle
        // * add the given BinPackRectangle to this bin
        // * may modify the position and orientation of
        //   the rectangle
        // * returns false if there wasn't enough space
        //   to place the rectangle
        bool AddRectangle(BinPackRectangle &rect);

    private:
        uint m_width;
        uint m_height;

        // The origin (0,0) is defined as the
        // bottom left corner of the bin
        uint m_place_x;
        uint m_place_y;
        uint m_shelf_y;

        // Spacing used in x and y between
        // adjacent rectangles
        uint m_spacing;
    };
}


#endif // KS_BINPACK_SHELF_HPP
