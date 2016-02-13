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

#ifndef KS_RANGE_ALLOCATOR_HPP
#define KS_RANGE_ALLOCATOR_HPP

#include <vector>
#include <list>
#include <algorithm>

#include <ks/KsLog.hpp>
#include <ks/KsException.hpp>

namespace ks
{   
    class RequestedEmptyRange : public ks::Exception
    {
    public:
        RequestedEmptyRange(std::string msg) :
            ks::Exception(ks::Exception::ErrorLevel::ERROR,std::move(msg)) {}

        ~RequestedEmptyRange() = default;
    };

    // T should be copyable
    template<typename DataT,typename IndexT=uint>
    class RangeAllocator
    {
        static_assert(std::is_integral<IndexT>::value,
                      "ERROR: ks: RangeAllocator: "
                      "IndexT must be an integral type");

    public:
        struct Block;

        using BlockListIterator =
            typename std::list<Block>::iterator;

        using BlockListConstIterator =
            typename std::list<Block>::iterator;

        struct Range
        {
            IndexT start;
            IndexT size;
            typename std::list<Block>::iterator block;
        };

        struct Block
        {
            DataT data;
            IndexT used_count;
            std::vector<Range> list_avail;
        };


        RangeAllocator(IndexT block_size,
                       IndexT range_count_hint=0) :
            m_block_size(block_size),
            m_range_count_hint(range_count_hint)
        {

        }

        ~RangeAllocator()
        {

        }

        IndexT GetBlockSize() const
        {
            return m_block_size;
        }

        IndexT GetBlockCount() const
        {
            return m_list_blocks.size();
        }

        std::list<Block> const & GetBlockList() const
        {
            return m_list_blocks;
        }

        BlockListConstIterator CreateBlock(DataT block_data)
        {
            auto new_block =
                    m_list_blocks.insert(
                        m_list_blocks.end(),
                        Block{
                            block_data,
                            0,
                            {}
                        });

            if(m_range_count_hint > 0) {
                new_block->list_avail.reserve(m_range_count_hint);
            }

            // add the initial range
            new_block->list_avail.push_back(
                        Range{0,m_block_size,new_block});

            return new_block;
        }

        void RemoveBlock(BlockListConstIterator it)
        {
            m_list_blocks.erase(it);
        }

        Range AcquireRange(uint size)
        {
            // TODO throw if size==0
            if(size==0) {
                throw RequestedEmptyRange("");
            }

            for(auto block_it = m_list_blocks.begin();
                block_it != m_list_blocks.end(); ++block_it)
            {
                Block &block = *block_it;

                for(auto range_it = block.list_avail.begin();
                    range_it != block.list_avail.end(); ++range_it)
                {
                    if(range_it->size > size)
                    {
                        // split the range
                        Range range_used{
                            range_it->start,
                            size,
                            block_it
                        };

                        Range range_keep{
                            range_it->start+size,
                            range_it->size-size,
                            block_it
                        };

                        block.list_avail.erase(range_it);

                        // ordered insert of split range
                        listAvailOrderedInsert(block.list_avail,range_keep);
                        block.used_count++;

                        return range_used;
                    }
                    else if(range_it->size == size)
                    {
                        // no need to split the range, just
                        // remove it from the avail list
                        Range range_used = *range_it;
                        block.list_avail.erase(range_it);
                        block.used_count++;

                        return range_used;
                    }
                }
            }

            // If we get here it means @size exceeds block size
            // or that all blocks are full
            Range null_range;
            null_range.start = 0;
            null_range.size = 0;
            return null_range;
        }

        void ReleaseRange(Range const &range, bool& empty)
        {
            // ordered insert back into list_avail
            range.block->used_count--;
            listAvailOrderedInsert(
                        range.block->list_avail,
                        range);

            empty = (range.block->used_count == 0);
        }

        void ClearAllRanges()
        {
            m_list_blocks.clear();
        }

    private:
        static void listAvailOrderedInsert(std::vector<Range> &list_avail,
                                           Range const &range)
        {
            if(list_avail.empty()) {
                list_avail.push_back(range);
                return;
            }

            // upper_bound: returns first element greater
            // than value or list_avail.end if no greater
            // element is found
            auto it_next = std::upper_bound(
                        list_avail.begin(),
                        list_avail.end(),
                        range,
                        [](Range const &a, Range const &b){
                            return (a.start < b.start);
                        });

            bool insert_range = true;

            // Try to merge the range before inserting it in the list
            if(it_next == list_avail.begin()) {
                // * @range would go at the beginning of list_avail
                // * check if we can merge with the next range (it_next)
                if(it_next->start == (range.start+range.size)) {
                    // we can merge with the next range (it_next)
                    it_next->start = range.start;
                    it_next->size += range.size;
                    insert_range = false;
                }
            }
            else if(it_next == list_avail.end()) {
                // * @range would go at the end of list_avail
                // * check if we can merge with the preceding range
                auto it_prev = std::prev(it_next);
                if(range.start == (it_prev->start+it_prev->size)) {
                    // we can merge with the prev range (it_prev)
                    it_prev->size += range.size;
                    insert_range = false;
                }
            }
            else { // most likely case
                // * @range has an element in front of it and
                //   preceding it, we need to check both
                auto it_prev = std::prev(it_next);
                if(range.start == (it_prev->start+it_prev->size)) { // merge prev
                    if(it_next->start == (range.start+range.size)) { // merge next
                        it_prev->size += (range.size+it_next->size);
                        list_avail.erase(it_next);
                        insert_range = false;
                    }
                    else { // merge prev only
                        it_prev->size += range.size;
                        insert_range = false;
                    }
                }
                else if(it_next->start == (range.start+range.size)) { // merge next only
                    it_next->start = range.start;
                    it_next->size += range.size;
                    insert_range = false;
                }
            }

            if(insert_range) {
                list_avail.insert(it_next,range);
            }
        }


        IndexT m_block_size;
        IndexT m_range_count_hint;
        std::list<Block> m_list_blocks;
    };
}

#endif // KS_RANGE_ALLOCATOR_HPP
