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

#include <ks/shared/KsRecycleIndexList.hpp>

template<typename T>
bool IndexVectorCompare(std::vector<T> const &list_a,
                        std::vector<T> const &list_b)
{
    if(list_a.size() != list_b.size()) {
        return false;
    }

    for(uint i=0; i < list_a.size(); i++) {
        if(list_a[i] != list_b[i]) {
            return false;
        }
    }

    return true;
}

TEST_CASE("RecycleIndexList","[recycleindexlist]")
{
    SECTION("Add")
    {
        ks::RecycleIndexList<std::string> list;

        uint a = list.Add("a"); // 0
        REQUIRE(list.GetList().size() == 1);
        REQUIRE(list.GetValid(a));
        REQUIRE(list.GetListAvail().empty());
        REQUIRE(list[a] == std::string("a"));

        uint b = list.Add("b");
        uint c = list.Add("c");
        uint d = list.Add("d");
        uint e = list.Add("e"); // 4
        REQUIRE(list.GetList().size() == 5);
        REQUIRE(list.GetListAvail().empty());

        bool ok = true;

        for(uint i=0; i < 5; i++) {
            ok = ok && (list.GetValid(i));
        }

        ok =
                ok &&
                (list[b] == "b") &&
                (list[c] == "c") &&
                (list[d] == "d") &&
                (list[e] == "e");

        REQUIRE(ok);
    }

    SECTION("Remove")
    {
        ks::RecycleIndexList<std::string> list;

        list.Add("a");
        list.Add("b");
        list.Add("c");
        list.Add("d");
        list.Add("e");
        // Expect:
        // index: 0 1 2 3 4
        // value: a b c d e
        // valid: y y y y y
        // avail: empty

        list.Remove(1);
        list.Remove(3);
        // Expect:
        // index: 0 1 2 3 4
        // value: a ? c ? e
        // valid: y n y n y
        // avail: 1,3
        REQUIRE(list.GetValid(1) == false);
        REQUIRE(list.GetValid(3) == false);
        REQUIRE(list.GetListAvail().size() == 2);
        REQUIRE(list.GetListAvail()[0] == 1);
        REQUIRE(list.GetListAvail()[1] == 3);

        list.Remove(4);
        // (should automagically remove invalid elements
        // at the end of the list)
        // Expect:
        // index: 0 1 2
        // value: a ? c
        // valid: y n y
        // avail: 1
        REQUIRE(list.GetList().size() == 3);
        REQUIRE(list.GetListAvail().size() == 1);
        REQUIRE(list.GetListAvail()[0] == 1);
    }

    SECTION("Peek next index")
    {
        ks::RecycleIndexList<std::string> list;

        uint a = list.Add("a");
        uint b = list.Add("b");
        uint c = list.Add("c");
        uint d = list.Add("d");
        uint e = list.Add("e");

        REQUIRE(list.PeekNextIndex() == 5);

        list.Remove(b); // 1
        list.Remove(c); // 2

        REQUIRE(list.PeekNextIndex() == 2);
    }

    SECTION("Recycle")
    {
        ks::RecycleIndexList<std::string> list;

        list.Add("a");
        list.Add("b");
        list.Add("c");
        list.Add("d");
        list.Add("e");
        // Expect:
        // index: 0 1 2 3 4
        // value: a b c d e
        // valid: y y y y y
        // avail: empty

        list.Remove(0);
        // Expect:
        // index: 0 1 2 3 4
        // value: ? b c d e
        // valid: n y y y y
        // avail: 0

        list.Add("x");
        // Expect:
        // Fill in the avail slot:
        // index: 0 1 2 3 4
        // value: z b c d e
        // valid: y y y y y
        // avail: empty
        REQUIRE(IndexVectorCompare<std::string>(
                    list.GetList(),
                    {{"x"},{"b"},{"c"},{"d"},{"e"}}));

        list.Remove(1);
        list.Remove(3);
        list.Remove(4);

        list.Add("y");
        list.Add("z");
        list.Add("w");

        REQUIRE(IndexVectorCompare<std::string>(
                    list.GetList(),
                    {{"x"},{"y"},{"c"},{"z"},{"w"}}));

        list.Remove(0);
        list.Add("p");

        REQUIRE(IndexVectorCompare<std::string>(
                    list.GetList(),
                    {{"p"},{"y"},{"c"},{"z"},{"w"}}));
    }
}
