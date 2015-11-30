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

#include <ks/shared/KsBinPackShelf.hpp>

namespace ks
{
    BinPackShelf::BinPackShelf(uint width,
                               uint height,
                               uint spacing) :
        m_width(width),
        m_height(height),
        m_spacing(spacing)
    {
        // start the initial placement at
        // the top left corner
        m_place_x = 0;
        m_place_y = 0;
        m_shelf_y = 0;
    }

    uint BinPackShelf::GetWidth() const
    {
        return m_width;
    }

    uint BinPackShelf::GetHeight() const
    {
        return m_height;
    }

    bool BinPackShelf::AddRectangle(BinPackRectangle &rect)
    {
        rect.x = m_place_x+m_spacing;
        rect.y = m_place_y+m_spacing;
        uint rect_right = rect.x+rect.width;
        uint rect_bottom = rect.y+rect.height;

        if(rect_bottom < m_height) {
            if(rect_right < m_width) {
                // the rectangle fits

                // advance place
                m_place_x = rect_right;

                // adjust shelf
                if(m_shelf_y < rect_bottom) {
                    m_shelf_y = rect_bottom;
                }

                return true;
            }
            else {
                // try jumping up a shelf
                m_place_x = 0;
                m_place_y = m_shelf_y;
                rect.x = m_place_x + m_spacing; // left
                rect.y = m_place_y + m_spacing; // bottom
                rect_right = rect.x + rect.width;
                rect_bottom = rect.y + rect.height;

                // check if the new placement
                // allows the rectangle to fit
                if(rect_bottom < m_height) {
                    if(rect_right < m_width) {
                        // the rectangle fits;

                        // advance place
                        m_place_x = rect_right;

                        // adjust shelf
                        if(m_shelf_y < rect_bottom) {
                            m_shelf_y = rect_bottom;
                        }

                        return true;
                    }
                }
            }
        }
        return false;
    }
}
