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
#include <ks/KsLog.hpp>
#include <ks/shared/KsGraph.hpp>

template<typename T>
bool CheckDFTPreOrder(ks::Graph<T> &graph,
                      uint start_index,
                      std::vector<T> const &list_check)
{
    auto list_ix = graph.GetDFTPreOrder(start_index);

    if(list_ix.size() != list_check.size()) {
        return false;
    }

    for(uint i=0; i < list_ix.size(); i++) {
        if(graph.GetNode(list_ix[i]).value != list_check[i]) {
            return false;
        }
    }

    return true;
}

template<typename T>
bool CheckTopoSort(ks::Graph<T> &graph,
                   std::vector<T> const &list_check)
{
    auto list_ix = graph.GetTopologicallySorted();

    if(list_ix.size() != list_check.size()) {
        return false;
    }

    for(uint i=0; i < list_ix.size(); i++) {
        if(graph.GetNode(list_ix[i]).value != list_check[i]) {
            return false;
        }
    }

    return true;
}

template<typename T>
bool CheckIndicesContainNodes(ks::Graph<T> &graph,
                              std::vector<uint> const &list_indices,
                              std::vector<T> const &list_check)
{
    for(auto &s : list_check) {
        bool ok = false;
        for(auto index : list_indices) {
            if(graph.GetNode(index).value == s) {
                ok = true;
                break;
            }
        }

        if(!ok) {
            return false;
        }
    }

    return true;
}

template<typename T>
bool CheckIndicesMatchList(ks::Graph<T> &graph,
                           std::vector<uint> const &list_indices,
                           std::vector<T> const &list_check)
{
    if(list_indices.size() != list_check.size()) {
        return false;
    }

    for(uint i=0; i < list_indices.size(); i++) {
        if(graph.GetNode(list_indices[i]).value != list_check[i]) {
            return false;
        }
    }

    return true;
}

TEST_CASE("Graph","[graph]")
{
    SECTION("Test Add/Remove Nodes Only")
    {
        ks::Graph<std::string,uint> graph;

        auto ix_a = graph.AddNode("a");
        REQUIRE(graph.GetNode(ix_a).value == "a");
        REQUIRE(graph.GetNode(ix_a).state == 0);
        REQUIRE(graph.GetNode(ix_a).inputs.empty());
        REQUIRE(graph.GetNode(ix_a).outputs.empty());

        auto ix_b = graph.AddNode("b");
        auto ix_c = graph.AddNode("c");
        auto ix_d = graph.AddNode("d");
        auto ix_e = graph.AddNode("e");

        REQUIRE(graph.GetNode(ix_b).value == "b");
        REQUIRE(graph.GetNode(ix_c).value == "c");
        REQUIRE(graph.GetNode(ix_d).value == "d");
        REQUIRE(graph.GetNode(ix_e).value == "e");

        SECTION("Remove Nodes")
        {
            graph.RemoveNode(ix_a,false);
            graph.RemoveNode(ix_c,false);
            graph.RemoveNode(ix_e,false);
            REQUIRE(graph.GetNode(ix_a).valid == false);
            REQUIRE(graph.GetNode(ix_b).valid == true);
            REQUIRE(graph.GetNode(ix_c).valid == false);
            REQUIRE(graph.GetNode(ix_d).valid == true);
            REQUIRE(graph.GetNode(ix_e).valid == false);
        }
    }

    SECTION("Test Add/Remove Edges Only")
    {
        ks::Graph<std::string,uint> graph;

        auto a = graph.AddNode("a");
        auto b = graph.AddNode("b");
        auto c = graph.AddNode("c");
        auto d = graph.AddNode("d");
        auto e = graph.AddNode("e");

        // Create a chain a->b->c->d->e
        graph.AddEdge(a,b);
        graph.AddEdge(b,c);
        graph.AddEdge(c,d);
        graph.AddEdge(d,e);

        REQUIRE(CheckDFTPreOrder<std::string>(
                    graph,a,
                    {{"a"},{"b"},{"c"},{"d"},{"e"}}));

        // Remove a single edge c->d
        graph.RemoveEdge(c,d);
        REQUIRE(CheckDFTPreOrder<std::string>(
                    graph,a,
                    {{"a"},{"b"},{"c"}}));

        // Remove all edges
        graph.ClearEdges();
        REQUIRE(CheckDFTPreOrder<std::string>(
                    graph,a,
                    {{"a"}}));

        // Create a network with multiple inputs and outputs
        auto p = graph.AddNode("p");
        graph.AddEdge(p,a);
        graph.AddEdge(p,b);
        graph.AddEdge(p,c);
        graph.AddEdge(a,c);
        graph.AddEdge(b,c);
        graph.AddEdge(c,d);
        REQUIRE(CheckDFTPreOrder<std::string>(
                    graph,p,
                    {{"p"},{"a"},{"c"},{"d"},{"b"},{"c"},{"d"},{"c"},{"d"}}));

        // Remove edges (p->c and b->c)
        graph.RemoveEdge(p,c);
        graph.RemoveEdge(b,c);
        REQUIRE(CheckDFTPreOrder<std::string>(
                    graph,p,
                    {{"p"},{"a"},{"c"},{"d"},{"b"}}));
    }

    SECTION("Test Add/Remove Nodes with Edges")
    {
        ks::Graph<std::string,uint> graph;

        auto p = graph.AddNode("p");
        auto a = graph.AddNode("a");
        auto b = graph.AddNode("b");
        auto c = graph.AddNode("c");
        auto d = graph.AddNode("d");
        auto e = graph.AddNode("e");

        graph.AddEdge(p,a);
        graph.AddEdge(a,b);
        graph.AddEdge(b,c);
        graph.AddEdge(c,d);
        graph.AddEdge(d,e);

        // Remove B from the chain p->a->b->c->d->e without
        // removing any orphaned nodes
        graph.RemoveNode(b,false);

        REQUIRE(CheckDFTPreOrder<std::string>(
                    graph,p,
                    {{"p"},{"a"}}));

        REQUIRE(CheckDFTPreOrder<std::string>(
                    graph,c,
                    {{"c"},{"d"},{"e"}}));

        // Re-add B to the chain
        b = graph.AddNode("b");
        graph.AddEdge(a,b);
        graph.AddEdge(b,c);

        // Remove B from the chain P->A->B->C->D->E but
        // this time remove any orphaned nodes
        graph.RemoveNode(b,true);

        REQUIRE(CheckDFTPreOrder<std::string>(
                    graph,p,
                    {{"p"},{"a"}}));

        REQUIRE(graph.GetNode(b).valid == false);
        REQUIRE(graph.GetNode(c).valid == false);
        REQUIRE(graph.GetNode(d).valid == false);
        REQUIRE(graph.GetNode(e).valid == false);

        // Clear all and re-add nodes
        graph.ClearAll();
        p = graph.AddNode("p");
        a = graph.AddNode("a");
        b = graph.AddNode("b");
        c = graph.AddNode("c");
        d = graph.AddNode("d");
        e = graph.AddNode("e");

        // Create network with multiple inputs and outputs
        graph.AddEdge(p,a);
        graph.AddEdge(a,c);
        graph.AddEdge(c,d);
        graph.AddEdge(c,e);
        graph.AddEdge(p,b);
        graph.AddEdge(b,a);
        graph.AddEdge(b,c);

        // Remove 'a'
        graph.RemoveNode(a,true);
        REQUIRE(CheckDFTPreOrder<std::string>(
                    graph,p,
                    { {"p"},{"b"},{"c"},{"d"},{"e"} }));

        // Remove 'b'
        graph.RemoveNode(b,true);
        REQUIRE(CheckDFTPreOrder<std::string>(
                    graph,p,
                    { {"p"} }));
    }

    SECTION("Test Topological Sort")
    {
        ks::Graph<std::string,uint> graph;

        // Topological sort on a single subgraph
        auto a = graph.AddNode("a");
        auto b = graph.AddNode("b");
        auto c = graph.AddNode("c");
        auto d = graph.AddNode("d");
        auto e = graph.AddNode("e");
        auto f = graph.AddNode("f");
        auto g = graph.AddNode("g");
        auto h = graph.AddNode("h");

        graph.AddEdge(a,d);
        graph.AddEdge(a,e);
        graph.AddEdge(b,d);
        graph.AddEdge(c,e);
        graph.AddEdge(c,h);
        graph.AddEdge(d,f);
        graph.AddEdge(d,g);
        graph.AddEdge(d,h);
        graph.AddEdge(e,g);

        REQUIRE(CheckTopoSort<std::string>(
                    graph,
                    { {"c"},{"b"},{"a"},{"e"},{"d"},{"h"},{"g"},{"f"} }));

        // Topological sort on a multiple subgraphs
        // (x,y,z,w) form a separate subgraph
        auto x = graph.AddNode("x");
        auto y = graph.AddNode("y");
        auto z = graph.AddNode("z");
        auto w = graph.AddNode("w");

        graph.AddEdge(x,y);
        graph.AddEdge(x,z);
        graph.AddEdge(y,z);
        graph.AddEdge(y,w);
        graph.AddEdge(w,z);

        REQUIRE(CheckTopoSort<std::string>(
                    graph,
                    {
                        {"x"},{"y"},{"w"},{"z"},
                        {"c"},{"b"},{"a"},{"e"},{"d"},{"h"},{"g"},{"f"}
                    }));
    }

//    SECTION("Whatever")
//    {
//        ks::Graph<std::string,uint> graph;

//                auto a = graph.AddNode("a");
//                auto b = graph.AddNode("b");
//                auto c = graph.AddNode("c");
//                auto d = graph.AddNode("d");
//                auto e = graph.AddNode("e");
//                auto f = graph.AddNode("f");
////                auto g = graph.AddNode("g");
////                auto h = graph.AddNode("h");
////                auto i = graph.AddNode("i");
////                auto j = graph.AddNode("j");
////                auto k = graph.AddNode("k");
////                auto l = graph.AddNode("l");
////                auto m = graph.AddNode("m");
////                auto n = graph.AddNode("n");

//                // subgraph 1
//                graph.AddEdge(a,b);
//                graph.AddEdge(b,c);
//                graph.AddEdge(c,d);
//                graph.AddEdge(b,d);

//                // subgraph 2
//                graph.AddEdge(e,f);

//                // subgraph 3
////                graph.AddEdge(g,h);
////                graph.AddEdge(h,i);
////                graph.AddEdge(i,k);
////                graph.AddEdge(k,n);
////                graph.AddEdge(h,j);
////                graph.AddEdge(j,k);
////                graph.AddEdge(j,l);
////                graph.AddEdge(l,n);
////                graph.AddEdge(l,m);

//        auto toposort = graph.GetDisjointSubgraphs(true);
////        ks::LOG.Trace() << "sz " << toposort.size();
////        for(uint i=0; i < toposort.size(); i++) {
////            ks::LOG.Trace() << toposort[i];
////        }
//    }

    SECTION("Test finding disjoint subgraphs")
    {
        ks::Graph<std::string,uint> graph;

        auto a = graph.AddNode("a");
        auto b = graph.AddNode("b");
        auto c = graph.AddNode("c");
        auto d = graph.AddNode("d");
        auto e = graph.AddNode("e");
        auto f = graph.AddNode("f");
        auto g = graph.AddNode("g");
        auto h = graph.AddNode("h");
        auto i = graph.AddNode("i");
        auto j = graph.AddNode("j");
        auto k = graph.AddNode("k");
        auto l = graph.AddNode("l");
        auto m = graph.AddNode("m");
        auto n = graph.AddNode("n");

        // subgraph 1
        graph.AddEdge(a,b);
        graph.AddEdge(b,c);
        graph.AddEdge(c,d);
        graph.AddEdge(b,d);

        // subgraph 2
        graph.AddEdge(e,f);

        // subgraph 3
        graph.AddEdge(g,h);
        graph.AddEdge(h,i);
        graph.AddEdge(i,k);
        graph.AddEdge(k,n);
        graph.AddEdge(h,j);
        graph.AddEdge(j,k);
        graph.AddEdge(j,l);
        graph.AddEdge(l,n);
        graph.AddEdge(l,m);

        SECTION("Get disjoint subgraphs")
        {
            auto list_subgraph_indices =
                    graph.GetDisjointSubgraphs();

            REQUIRE(list_subgraph_indices.size() == 3);

            uint sg0;
            uint sg1;
            uint sg2;
            for(uint i=0; i < list_subgraph_indices.size(); i++) {
                uint const sg_size =
                        list_subgraph_indices[i].size();

                if(sg_size == 4) {
                    sg0 = i;
                }
                else if(sg_size == 2) {
                    sg1 = i;
                }
                else if(sg_size == 8) {
                    sg2 = i;
                }
            }

            REQUIRE(CheckIndicesContainNodes<std::string>(
                        graph,
                        list_subgraph_indices[sg0],
                        {
                            {"a"},{"b"},{"c"},{"d"}
                        }
                        ));

            REQUIRE(CheckIndicesContainNodes<std::string>(
                        graph,
                        list_subgraph_indices[sg1],
                        {
                            {"e"},{"f"}
                        }
                        ));

            REQUIRE(CheckIndicesContainNodes<std::string>(
                        graph,
                        list_subgraph_indices[sg2],
                        {
                            {"g"},{"h"},{"i"},{"j"},{"k"},{"l"},{"m"},{"n"}
                        }
                        ));
        }

        SECTION("Get topographically sorted disjoint subgraphs")
        {
            auto list_subgraph_indices =
                    graph.GetDisjointSubgraphs(true);

            REQUIRE(list_subgraph_indices.size() == 3);

            uint sg0;
            uint sg1;
            uint sg2;
            for(uint i=0; i < list_subgraph_indices.size(); i++) {
                uint const sg_size =
                        list_subgraph_indices[i].size();

                if(sg_size == 4) {
                    sg0 = i;
                }
                else if(sg_size == 2) {
                    sg1 = i;
                }
                else if(sg_size == 8) {
                    sg2 = i;
                }
            }

            // NOTE:
            // Topological sorts aren't unique so these
            // are just results that 'should' happen based
            // on the order the nodes were added to graph
            // and the current topological sort implementation
            // in the Graph class.

            // If these fail, the sort might still be valid
            // but the order of node addition or the topological
            // sort function may have changed

            REQUIRE(CheckIndicesMatchList<std::string>(
                        graph,
                        list_subgraph_indices[sg0],
                        {
                            {"a"},{"b"},{"c"},{"d"}
                        }
                        ));

            REQUIRE(CheckIndicesMatchList<std::string>(
                        graph,
                        list_subgraph_indices[sg1],
                        {
                            {"e"},{"f"}
                        }
                        ));

            REQUIRE(CheckIndicesMatchList<std::string>(
                        graph,
                        list_subgraph_indices[sg2],
                        {
                            {"g"},{"h"},{"i"},{"j"},{"k"},{"l"},{"m"},{"n"}
                        }
                        ));

        }
    }
}



















