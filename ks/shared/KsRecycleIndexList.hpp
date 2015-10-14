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

#ifndef KS_RECYCLE_INDEX_LIST_HPP
#define KS_RECYCLE_INDEX_LIST_HPP

#include <vector>
#include <algorithm>
#include <ks/KsGlobal.hpp>
#include <ks/KsLog.hpp>

namespace ks
{
    // ============================================================= //

    enum class RecycleIndexListRemovalPolicy : u8 {
        Resize,
        Shrink,
        None
    };

    // ============================================================= //

    // Forward declarations
    template<typename ListT,
             typename IndexT,
             RecycleIndexListRemovalPolicy Policy>
    class RecycleIndexList;

    // ============================================================= //

    namespace rec_idx_list_detail
    {
        using Policy = RecycleIndexListRemovalPolicy;

        template<typename T,typename I,Policy P>
        struct RemoveHelper{};

        template<typename T,typename I>
        struct RemoveHelper<T,I,Policy::Resize>
        {
            static void remove(RecycleIndexList<T,I,Policy::Resize>* list, I index)
            {
                list->m_count--;
                list->m_list[index]=T();
                list->m_list_valid[index]=list->k_invalid;

                // insert in sorted list
                auto ins_avail_it =
                        std::upper_bound(list->m_list_avail.begin(),
                                         list->m_list_avail.end(),
                                         index);

                list->m_list_avail.insert(ins_avail_it,index);

                // trim
                while(list->m_list_valid.back() == list->k_invalid) {
                    list->m_list.pop_back();
                    list->m_list_valid.pop_back();
                    list->m_list_avail.pop_back();

                    if(list->m_list_valid.empty()) {
                        break;
                    }
                }
            }
        };

        template<typename T,typename I>
        struct RemoveHelper<T,I,Policy::None>
        {
            static void remove(RecycleIndexList<T,I,Policy::None>* list, I index)
            {
                list->m_count--;
                list->m_list[index]=T();
                list->m_list_valid[index]=list->k_invalid;

                list->m_list_avail.push_back(index);
            }
        };
    }

    // ============================================================= //
    // ============================================================= //

    template<typename T,
             typename IndexT=uint,
             RecycleIndexListRemovalPolicy PolicyT=
                RecycleIndexListRemovalPolicy::Resize>
    class RecycleIndexList final
    {
        static_assert(std::is_integral<IndexT>::value,
                      "ERROR: ks: RecycleIndexList: "
                      "IndexT must be an integral type");

        template<typename L,
                 typename I,
                 RecycleIndexListRemovalPolicy P>
        friend struct rec_idx_list_detail::RemoveHelper;

    public:
        IndexT Add(T val)
        {
            m_count++;
            if(m_list_avail.empty()) {
                m_list.push_back(std::move(val));
                m_list_valid.push_back(k_valid);
                return (m_list.size()-1);
            }

            auto const index = m_list_avail.back();
            m_list[index] = std::move(val);
            m_list_valid[index] = k_valid;
            m_list_avail.pop_back();

            return index;
        }

        void Remove(IndexT index)
        {
            rec_idx_list_detail::RemoveHelper<
                    T,IndexT,PolicyT>::remove(this,index);
        }

        IndexT PeekNextIndex() const
        {
            if(m_list_avail.empty()) {
                return (static_cast<IndexT>(m_list.size()));
            }

            return (m_list_avail.back());
        }

        void Reserve(uint count)
        {
            m_list.reserve(count);
            m_list_valid.reserve(count);
            m_list_avail.reserve(count);
        }

        void ShrinkToFit()
        {
            m_list.shrink_to_fit();
            m_list_valid.shrink_to_fit();
            m_list_avail.shrink_to_fit();
        }

        void Clear()
        {
            m_list.clear();
            m_list_valid.clear();
            m_list_avail.clear();
        }

        T& Get(IndexT index)
        {
            return m_list[index];
        }

        T const & Get(IndexT index) const
        {
            return m_list[index];
        }

        IndexT GetCount() const
        {
            return m_count;
        }

        bool GetValid(IndexT index)
        {
            return m_list_valid[index];
        }

        T& operator[] (IndexT index)
        {
            return m_list[index];
        }

        T const & operator[] (IndexT index) const
        {
            return m_list[index];
        }

        std::vector<T>& GetList()
        {
            return m_list;
        }

        std::vector<T> const& GetList() const
        {
            return m_list;
        }

        std::vector<IndexT> const & GetListAvail() const
        {
            return m_list_avail;
        }

    private:
        u8 k_valid{1};
        u8 k_invalid{0};
        IndexT m_count{0};
        std::vector<T> m_list; // sparse
        std::vector<u8> m_list_valid;

        // keep sorted if remove policy is resize
        std::vector<IndexT> m_list_avail;
    };

    // ============================================================= //
}


#endif // KS_RECYCLE_INDEX_LIST_HPP
