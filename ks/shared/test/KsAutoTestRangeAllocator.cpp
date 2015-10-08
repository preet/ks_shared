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

#include <catch/catch.hpp>
#include <random>
#include <ks/KsLog.hpp>
#include <ks/shared/KsRangeAllocator.hpp>


TEST_CASE("RangeAllocator","[rangeallocator]")
{
    SECTION("Construction")
    {
        ks::RangeAllocator<ks::uint> rac(100);

        SECTION("Acquire Range / No Blocks")
        {
            auto r = rac.AcquireRange(10);
            REQUIRE(r.size==0);
        }

        SECTION("Create Block")
        {
            auto it_b0 = rac.CreateBlock(0);
            REQUIRE(it_b0->data == 0);

            SECTION("Acquire Range > Block Size")
            {
                auto r = rac.AcquireRange(1000);
                REQUIRE(r.size==0);
            }


            SECTION("Acquire Range == Block Size")
            {
                auto r0 = rac.AcquireRange(100);
                REQUIRE(r0.start == 0);
                REQUIRE(r0.size == 100);
                REQUIRE(r0.block->data == 0);

                // should be filled up
                r0 = rac.AcquireRange(100);
                REQUIRE(r0.size == 0);
            }

            SECTION("Acquire Range < Block Size")
            {
                auto r0 = rac.AcquireRange(25);
                REQUIRE(r0.start == 0);
                REQUIRE(r0.size == 25);
                REQUIRE(r0.block->data == 0);

                auto r1 = rac.AcquireRange(25);
                REQUIRE(r1.start == 25);
                REQUIRE(r1.size == 25);
                REQUIRE(r1.block->data == 0);

                // shouldn't have enough space
                auto temp0 = rac.AcquireRange(75);
                REQUIRE(temp0.size==0);

                auto r2 = rac.AcquireRange(50);
                REQUIRE(r2.start == 50);
                REQUIRE(r2.size == 50);
                REQUIRE(r2.block->data == 0);

                // should be filled up
                auto temp1 = rac.AcquireRange(1);
                REQUIRE(temp1.size==0);

                SECTION("Release Range")
                {
                    REQUIRE(it_b0->list_avail.size()==0);

                    bool empty;
                    rac.ReleaseRange(r2,empty);
                    REQUIRE(empty==false);
                    REQUIRE(it_b0->list_avail.size()==1);
                    REQUIRE(it_b0->list_avail.begin()->start == 50);
                    REQUIRE(it_b0->list_avail.begin()->size == 50);

                    // disjoint ranges shouldn't merge
                    rac.ReleaseRange(r0,empty);
                    REQUIRE(empty==false);
                    REQUIRE(it_b0->list_avail.size()==2);
                    REQUIRE(it_b0->list_avail.begin()->start == 0);
                    REQUIRE(it_b0->list_avail.begin()->size == 25);

                    // adjacent ranges should be merged
                    r0 = rac.AcquireRange(25);
                    REQUIRE(it_b0->list_avail.size()==1);
                    REQUIRE(it_b0->list_avail.begin()->start == 50);
                    REQUIRE(it_b0->list_avail.begin()->size == 50);

                    rac.ReleaseRange(r1,empty);
                    REQUIRE(it_b0->list_avail.size()==1);
                    REQUIRE(it_b0->list_avail.begin()->start == 25);
                    REQUIRE(it_b0->list_avail.begin()->size == 75);

                    auto rf = rac.AcquireRange(75);
                    REQUIRE(it_b0->list_avail.size()==0);

                    // check that empty flag is set when
                    // the block is completely emptied
                    rac.ReleaseRange(r0,empty);
                    REQUIRE(empty==false);

                    rac.ReleaseRange(rf,empty);
                    REQUIRE(empty);
                }
            }
        }
    }

    SECTION("Realistic usage")
    {
        uint const k_block_size=900;
        uint const k_indiv_size=15;
        uint const k_indiv_count=60;

        std::random_device rd;
        std::mt19937 rand_gen_mt(rd());

        ks::RangeAllocator<uint> range_alloc(k_block_size);
        std::vector<ks::RangeAllocator<uint>::Range> list_ranges;

        bool ok = true;

        for(uint i=0; i < k_indiv_count; i++)
        {
            auto range = range_alloc.AcquireRange(k_indiv_size);
            if(range.size == 0) {
                auto block_it = range_alloc.CreateBlock(0);
                range = range_alloc.AcquireRange(k_indiv_size);
                ok = ok && ((range.size == k_indiv_size) && (range.block == block_it));
            }

            list_ranges.push_back(range);
        }

        REQUIRE(ok);

        // randomize order of ranges
        std::shuffle(list_ranges.begin(),list_ranges.end(),rand_gen_mt);

        // release ranges
        for(auto const &range : list_ranges) {
            bool empty;
            range_alloc.ReleaseRange(range,empty);
        }

        // ensure they were merged
        auto block_it = range_alloc.GetBlockList().begin();
        REQUIRE(block_it->list_avail.size() == 1);
        REQUIRE(block_it->list_avail[0].start == 0);
        REQUIRE(block_it->list_avail[0].size == k_block_size);
    }
}
