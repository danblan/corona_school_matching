#ifndef CORONA_SCHOOL_MATCHING_MATCHING_H
#define CORONA_SCHOOL_MATCHING_MATCHING_H

#include "graph_creator.h"
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/edmonds_karp_max_flow.hpp>
#include <boost/graph/find_flow_cost.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/read_dimacs.hpp>

namespace CS {
    /**
     * Main routine to compute a matching of maximum cost.
     * @param gc The graph creator that describes the edges
     * @param matching_edges A vector (possibly empty) in which we write the matching edges.
     * @return The cost of the maximum cost matching that was found.
     */
    CostValue compute_max_cost_matching (GraphCreator & gc, std::vector<Edge> &matching_edges) {
        ///Initialize edge weights, capacities, residual capacities and the boost graph
        Graph g(gc.num_nodes());
        boost::property_map < Graph, boost::edge_capacity_t >::type capacity = boost::get(boost::edge_capacity, g);
        boost::property_map < Graph, boost::edge_residual_capacity_t >::type
                residual_capacity = get(boost::edge_residual_capacity, g);
        boost::property_map < Graph, boost::edge_reverse_t >::type rev = get(boost::edge_reverse, g);
        boost::property_map < Graph, boost::edge_weight_t >::type weight = get(boost::edge_weight, g);
        std::map<std::pair<ID, ID>, Graph::edge_descriptor> edge_to_edge_descriptor;
        ///Lambda function to add an edge in the boost graph (with its residual edge, weight and capacity).
        ///This way we avoid duplicate code. (capacity of the edge is set to 1 as default)
        auto add_edge = [&] (ID tail, ID head, double cost, unsigned cap = 1) {
            auto const edge_descriptor = boost::add_edge(tail, head, g).first;
            auto const reverse_edge_descriptor = boost::add_edge(head, tail, g).first;
            rev[edge_descriptor] = reverse_edge_descriptor;
            rev[reverse_edge_descriptor] = edge_descriptor;
            capacity[edge_descriptor] = cap;
            capacity[reverse_edge_descriptor] = 0;
            weight[edge_descriptor] = cost;
            //Cost of the reverse edge must be inverted!
            weight[reverse_edge_descriptor] = -cost;
            residual_capacity[edge_descriptor] = cap;
            residual_capacity[reverse_edge_descriptor] = 0;
            return edge_descriptor;
        };
        //Look at read from dimacs to see how it is done correctly
        for (auto const & edge : gc.edges()) {
            auto const pupil_idx = edge.pupil_id;
            auto const college_student_idx = edge.college_student_id + gc.nodes().pupils().size();
            ///Invert the edge weight, algorithm finds a min cost matchi
            /// !
            auto const edge_descriptor = add_edge(pupil_idx, college_student_idx, -edge.cost);
            edge_to_edge_descriptor[{edge.pupil_id, edge.college_student_id}] = edge_descriptor;
        }
        ///Add edges with 0 weight between s and pupils:
        for (auto const & pupil : gc.nodes().pupils()) {
            add_edge(gc.s_id(), pupil.id(), 0.);
        }
        ///Add edges with 0 weight between students and t:
        for (auto const & student : gc.nodes().college_students()) {
            auto const student_index = gc.nodes().pupils().size() + student.id();
            add_edge(student_index, gc.t_id(), 0.);
        }
        ///Add some dummy edges to make sure there is always a maximum flow of value 0:
        if (gc.nodes().pupils().size() <=  gc.nodes().college_students().size()) {
            for (auto const & pupil : gc.nodes().pupils()) {
                add_edge(pupil.id(), gc.t_id(), 0.);
            }
        } else {
            for (auto const & college_student : gc.nodes().college_students()) {
                auto const college_student_idx = gc.nodes().pupils().size() + college_student.id();
                add_edge(gc.s_id(), college_student_idx, 0.);
            }
        }
        //First find any maximum flow:
        boost::edmonds_karp_max_flow(g, gc.s_id(), gc.t_id()) ;
        //Then make it cost optimal:
        boost::cycle_canceling(g);
        //Retrieve the cost of the flow (maybe unneeded)
        int flow_cost = boost::find_flow_cost(g);
        ///Find the matching edges (those are the edges with 0 residual capacity between pupils and students):
        for (auto const &edge : gc.edges()) {
            auto const boost_edge = edge_to_edge_descriptor[{edge.pupil_id, edge.college_student_id}];
            std::cout<<residual_capacity[boost_edge]<<std::endl;
            ///If the residual capacity of the edge is 0, it is used by the algorithm.
            if (residual_capacity[boost_edge] == 0ul) {
                matching_edges.emplace_back(edge);
            }
        }
        //Invert the flow cost, because we computed a min cost flow for the negative costs.
        return -flow_cost;
    }
}

#endif //CORONA_SCHOOL_MATCHING_MATCHING_H
