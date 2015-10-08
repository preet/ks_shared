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

#ifndef KS_GRAPH_HPP
#define KS_GRAPH_HPP

#include <vector>
#include <algorithm>

#include <ks/KsGlobal.hpp>
#include <ks/shared/KsRecycleIndexList.hpp>

namespace ks
{
    template<typename T, typename Index=uint>
    class Graph final
    {
    public:
        struct Node {
            T value;

            bool valid;

            // helper for topological sort:
            // 0: unvisited
            // 1: visited
            // 2: finished
            u8 state;

            std::vector<Index> inputs;
            std::vector<Index> outputs;
        };

        Node const &GetNode(Index index)
        {
            if(m_list_nodes.GetList().size() > index) {
                return m_list_nodes[index];
            }
            return m_null_node;
        }

        std::vector<Node> const & GetSparseNodeList()
        {
            return m_list_nodes.GetList();
        }

        uint GetNodeCount() const
        {
            return m_node_count;
        }

        Index AddNode(T val)
        {
            m_node_count++;
            return m_list_nodes.Add(Node{std::move(val),true,0,{},{}});
        }

        void RemoveNode(Index node,bool rem_orphans)
        {
            // Remove all input edges
            auto &list_inputs = m_list_nodes[node].inputs;
            for(auto input : list_inputs) {
                auto &list_input_outputs = m_list_nodes[input].outputs;
                list_input_outputs.erase(
                            std::remove(
                                list_input_outputs.begin(),
                                list_input_outputs.end(),
                                node),
                            list_input_outputs.end());
            }
            list_inputs.clear();

            // Remove all output edges
            auto &list_outputs = m_list_nodes[node].outputs;
            for(auto output : list_outputs) {
                auto &list_output_inputs = m_list_nodes[output].inputs;
                list_output_inputs.erase(
                            std::remove(
                                list_output_inputs.begin(),
                                list_output_inputs.end(),
                                node),
                            list_output_inputs.end());

                // Recursively remove orphans if requested
                if(rem_orphans) {
                    if(list_output_inputs.empty()) {
                        RemoveNode(output,true);
                    }
                }
            }
            list_outputs.clear();

            m_list_nodes[node].valid = false;
            m_list_nodes.Remove(node);
            m_node_count--;
        }

        void AddEdge(Index const from, Index const to)
        {
            m_list_nodes[from].outputs.push_back(to);
            m_list_nodes[to].inputs.push_back(from);
        }

        void RemoveEdge(Index const from, Index const to)
        {
            // Remove 'to' from 'from's outputs
            auto &list_from_outputs = m_list_nodes[from].outputs;
            list_from_outputs.erase(
                        std::remove(
                            list_from_outputs.begin(),
                            list_from_outputs.end(),
                            to),
                        list_from_outputs.end());

            // Remove 'from' from 'to's inputs
            auto &list_to_inputs = m_list_nodes[to].inputs;
            list_to_inputs.erase(
                        std::remove(
                            list_to_inputs.begin(),
                            list_to_inputs.end(),
                            from),
                        list_to_inputs.end());
        }

        void ClearEdges()
        {
            auto &list_nodes = m_list_nodes.GetList();
            for(auto &node : list_nodes) {
                node.inputs.clear();
                node.outputs.clear();
            }
        }

        void ClearAll()
        {
            m_list_nodes.Clear();
        }

        // stateless naive traversal
        std::vector<Index> GetDFTPreOrder(Index start)
        {
            std::vector<Index> list_visit_order;
            dftPreOrder(start,m_list_nodes.GetList(),list_visit_order);

            return list_visit_order;
        }

        std::vector<Index> GetTopologicallySorted()
        {
            std::vector<Index> list_sorted_nodes;

            auto &list_nodes = m_list_nodes.GetList();

            // Reset visited state for all nodes
            for(Index i=0; i < list_nodes.size(); i++) {
                list_nodes[i].state = 0;
            }

            // Get Topological Sort
            for(Index i=0; i < list_nodes.size(); i++) {
                if(m_list_nodes.GetValid(i)) {
                    topologicalSort(i,list_nodes,list_sorted_nodes);
                }
            }

            // Reverse since topological sort adds results
            // in reverse order
            std::reverse(list_sorted_nodes.begin(),
                         list_sorted_nodes.end());

            return list_sorted_nodes;
        }

        std::vector<Index> GetDFSPreOrder()
        {
            std::vector<Index> list_visit_order;

            auto &list_nodes = m_list_nodes.GetList();

            // Reset visited state for all nodes
            for(Index i=0; i < list_nodes.size(); i++) {
                list_nodes[i].state = 0;
            }

            // Get traversal order
            for(Index i=0; i < list_nodes.size(); i++) {
                if(m_list_nodes.GetValid(i)) {
                    dfsPreOrder(i,list_nodes,list_visit_order);
                }
            }

            return list_visit_order;
        }

        std::vector<Index> GetDFSPostOrder()
        {
            std::vector<Index> list_visit_order;

            auto &list_nodes = m_list_nodes.GetList();

            // Reset visited state for all nodes
            for(Index i=0; i < list_nodes.size(); i++) {
                list_nodes[i].state = 0;
            }

            // Get traversal order
            for(Index i=0; i < list_nodes.size(); i++) {
                if(m_list_nodes.GetValid(i)) {
                    dfsPostOrder(i,list_nodes,list_visit_order);
                }
            }

            return list_visit_order;
        }

        std::vector<std::vector<Index>>
        GetDisjointSubgraphs(bool topologically_sorted=false)
        {
            std::vector<std::vector<Index>> list_subgraph_nodes;
            auto &list_nodes = m_list_nodes.GetList();

            // Reset visited state for all nodes
            for(Index i=0; i < list_nodes.size(); i++) {
                list_nodes[i].state = 0;
            }

            // Get subgraphs
            for(Index i=0; i < list_nodes.size(); i++) {
                std::vector<Index> list_visit_order;
                if(m_list_nodes.GetValid(i)) {
                    undirectedDfsPostOrder(i,list_nodes,list_visit_order);
                    if(!list_visit_order.empty()) {
                        list_subgraph_nodes.push_back(list_visit_order);
                    }
                }
            }

            // Sort subgraphs if required
            if(topologically_sorted)
            {
                std::vector<std::vector<Index>> list_sorted_subgraph_nodes;

                // Reset visited state for all nodes
                for(Index i=0; i < list_nodes.size(); i++) {
                    list_nodes[i].state = 0;
                }

                for(uint sg=0; sg < list_subgraph_nodes.size(); sg++)
                {
                    std::vector<Index> list_rev_sorted_nodes;

                    // Topological sort
                    for(Index i=0; i < list_subgraph_nodes[sg].size(); i++) {
                        if(m_list_nodes.GetValid(i)) {
                            topologicalSort(list_subgraph_nodes[sg][i],
                                            list_nodes,
                                            list_rev_sorted_nodes);
                        }
                    }

                    // Reverse since topological sort adds results
                    // in reverse order
                    std::reverse(list_rev_sorted_nodes.begin(),
                                 list_rev_sorted_nodes.end());

                    list_sorted_subgraph_nodes.push_back(
                                list_rev_sorted_nodes);
                }

                return list_sorted_subgraph_nodes;
            }

            return list_subgraph_nodes;
        }

    private:

        static void dftPreOrder(Index index,
                                std::vector<Node> &list_nodes,
                                std::vector<Index> &list_visit_order)
        {
            list_visit_order.push_back(index);

            for(Index output : list_nodes[index].outputs) {
                // visit all outputs
                dftPreOrder(output,list_nodes,list_visit_order);
            }
        }

        static void dfsPreOrder(Index index,
                                std::vector<Node> &list_nodes,
                                std::vector<Index> &list_visit_order)
        {
            // mark visited
            if(list_nodes[index].state != 0) {
                return;
            }
            list_nodes[index].state = 1;
            list_visit_order.push_back(index);

            for(Index output : list_nodes[index].outputs) {
                // visit all unvisited outputs
                if(list_nodes[output].state == 0) {
                    dfsPreOrder(output,list_nodes,list_visit_order);
                }
            }

            // mark traversal finished for this vertex
            list_nodes[index].state = 2;
        }

        static void dfsPostOrder(Index index,
                                 std::vector<Node> &list_nodes,
                                 std::vector<Index> &list_visit_order)
        {
            // mark visited
            if(list_nodes[index].state != 0) {
                return;
            }
            list_nodes[index].state = 1;

            for(Index output : list_nodes[index].outputs) {
                // visit all unvisited outputs
                if(list_nodes[output].state == 0) {
                    dfsPostOrder(output,list_nodes,list_visit_order);
                }
            }

            // mark traversal finished for this vertex
            list_nodes[index].state = 2;
            list_visit_order.push_back(index);
        }

        static void undirectedDfsPostOrder(Index index,
                                           std::vector<Node> &list_nodes,
                                           std::vector<Index> &list_visit_order)
        {
            // mark visited
            if(list_nodes[index].state != 0) {
                return;
            }
            list_nodes[index].state = 1;

            for(Index input : list_nodes[index].inputs) {
                // visit unvisited outputs
                if(list_nodes[input].state == 0) {
                    undirectedDfsPostOrder(input,list_nodes,list_visit_order);
                }
            }

            for(Index output : list_nodes[index].outputs) {
                // visit unvisited outputs
                if(list_nodes[output].state == 0) {
                    undirectedDfsPostOrder(output,list_nodes,list_visit_order);
                }
            }

            // mark traversal finished for this vertex
            list_nodes[index].state = 2;
            list_visit_order.push_back(index);
        }

        static void topologicalSort(Index index,
                                    std::vector<Node> &list_nodes,
                                    std::vector<Index> &list_rev_sorted_nodes)
        {
            // mark visited
            if(list_nodes[index].state != 0) {
                return;
            }
            list_nodes[index].state = 1;

            for(Index output : list_nodes[index].outputs) {
                // univisited, most likely case
                if(list_nodes[output].state == 0) {
                    topologicalSort(output,list_nodes,list_rev_sorted_nodes);
                }
                // already visited, indicates a cycle
                else if(list_nodes[output].state == 1) {
                    // TODO decide what to do here
                    continue;
                }
                // else: output vertex is finished, no
                // need to do anything
            }

            // mark traversal finished for this vertex
            list_nodes[index].state = 2;
            list_rev_sorted_nodes.push_back(index);
        }


        uint m_node_count{0};
        Node m_null_node{T{},false,0,{},{}};
        RecycleIndexList<Node> m_list_nodes;
    };
}


#endif // KS_GRAPH_HPP
