#ifndef CORONA_SCHOOL_MATCHING_MATCHING_H
#define CORONA_SCHOOL_MATCHING_MATCHING_H

#include "graph_creator.h"
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/read_dimacs.hpp>

namespace CS {

    enum MatchingAlgorithm {
        CycleCanceling,
        SuccessiveShortestPath
    };

    /**
     * This function provides a heuristic which computes a matching with high cost.
     * It sorts the edges by costs and adds the most expensive ones to the matching iteratively
     * if they maintain a feasible matching.
     * We use this as a starting point for the min cost matching algorithm in order to speed it up
     * (A good initial matching results in fewer negative cycles that have to be resolved).
     * @param gc The graph creator containing all edges.
     * @param matching_edges A vector in which the result of this heuristic will be stored.
     */
    void heuristic_max_cost_matching(GraphCreator const &gc, std::vector<Edge> &matching_edges) {
        std::vector<Edge> all_edges(gc.edges());
        auto greater_cost_comp = [](const Edge &lhs, const Edge &rhs) {
            return lhs.cost > rhs.cost;
        };
        std::vector<bool> is_pupil_covered(gc.nodes().pupils().size(), false);
        std::vector<bool> is_student_covered(gc.nodes().college_students().size(), false);
        //Sort all edges by cost (starting with the highest cost).
        std::sort(all_edges.begin(), all_edges.end(), greater_cost_comp);
        //Iterate over all edges and accept those where both endpoints are not covered.
        for (auto const &edge : all_edges) {
            if (is_pupil_covered[edge.pupil_id] or is_student_covered[edge.college_student_id]) {
                continue;
            } else {
                matching_edges.push_back(edge);
                is_pupil_covered[edge.pupil_id] = true;
                is_student_covered[edge.college_student_id] = true;
            }
        }
    }

    inline CostValue maximum_edge_weight(const std::vector<Edge> & edges) {
        CostValue retval = 0.;
        for (auto const & edge : edges) {
            retval = std::max(retval, edge.cost);
        }
        return retval;
    }

    template <typename Func>
    CostValue compute_max_cost_matching_cycle_canceling(const GraphCreator & gc,
                                                        std::map<std::pair<ID, ID>, Graph::edge_descriptor> & edge_to_edge_descriptor,
                                                        Graph & g,
                                                        boost::property_map<Graph, boost::edge_reverse_t>::type & rev,
                                                        boost::property_map<Graph, boost::edge_residual_capacity_t>::type & residual_capacity,
                                                        Func & add_edge) {
        ///For speedup we try to find a maximum flow that is already close to optimal via a heuristic:
        std::vector<Edge> heuristic_edges;
        heuristic_max_cost_matching(gc, heuristic_edges);
        ///Adapt the flow values on those edges by hand:
        for (auto const &edge : heuristic_edges) {
            auto const s_edge_descriptor = edge_to_edge_descriptor[{gc.s_id(), edge.pupil_id}];
            auto const edge_descriptor = edge_to_edge_descriptor[{edge.pupil_id, edge.college_student_id +
                                                                                 gc.nodes().pupils().size()}];
            auto const t_edge_descriptor = edge_to_edge_descriptor[{
                    edge.college_student_id + gc.nodes().pupils().size(), gc.t_id()}];
            residual_capacity[s_edge_descriptor] = 0;
            residual_capacity[edge_descriptor] = 0;
            residual_capacity[t_edge_descriptor] = 0;
            residual_capacity[rev[s_edge_descriptor]] = 1;
            residual_capacity[rev[edge_descriptor]] = 1;
            residual_capacity[rev[t_edge_descriptor]] = 1;
        }
        unsigned const remaining_flow_value =
                std::min(gc.nodes().pupils().size(), gc.nodes().college_students().size()) - heuristic_edges.size();
        ///Add a direct edge between s and t of cost 0 with the remaining flow value:
        add_edge(gc.s_id(), gc.t_id(), 0., remaining_flow_value);
        auto const boost_edge = edge_to_edge_descriptor[{gc.s_id(), gc.t_id()}];
        residual_capacity[boost_edge] = 0;
        residual_capacity[rev[boost_edge]] = remaining_flow_value;
        ///Now make the input flow cost optimal:
        boost::cycle_canceling(g);
        ///Retrieve the cost of the flow:
        CostValue flow_cost = boost::find_flow_cost(g);
        return flow_cost;
    }

    template <typename Func>
    CostValue compute_max_cost_matching_successive_shortest_paths(GraphCreator const & gc, Graph & g,
            Func & add_edge, CostValue additional_cost) {
        if (gc.nodes().pupils().size() < gc.nodes().college_students().size()) {
            for (auto const & pupil : gc.nodes().pupils()) {
                auto pupil_idx = pupil.id();
                add_edge(pupil_idx, gc.t_id(), additional_cost);
            }
        } else {
            for (auto const & student : gc.nodes().college_students()) {
                auto student_idx = student.id() + gc.nodes().pupils().size();
                add_edge(gc.s_id(), student_idx, additional_cost);
            }
        }
        boost::successive_shortest_path_nonnegative_weights(g, gc.s_id(), gc.t_id());
        CostValue  flow_cost = boost::find_flow_cost(g);
        return flow_cost;
    }

    /**
     * Main routine to compute a matching of maximum cost.
     * @param gc The graph creator that describes the edges
     * @param matching_edges A vector (possibly empty) in which we write the matching edges.
     * @return The cost of the maximum cost matching that was found.
     */
    CostValue compute_max_cost_matching(GraphCreator &gc, std::vector<Edge> &matching_edges, MatchingAlgorithm algorithm) {
        ///Initialize edge weights, capacities, residual capacities and the boost graph
        Graph g(gc.num_nodes());
        boost::property_map<Graph, boost::edge_capacity_t>::type capacity = boost::get(boost::edge_capacity, g);
        boost::property_map<Graph, boost::edge_residual_capacity_t>::type
                residual_capacity = get(boost::edge_residual_capacity, g);
        boost::property_map<Graph, boost::edge_reverse_t>::type rev = get(boost::edge_reverse, g);
        boost::property_map<Graph, boost::edge_weight_t>::type weight = get(boost::edge_weight, g);
        std::map<std::pair<ID, ID>, Graph::edge_descriptor> edge_to_edge_descriptor;
        CostValue additional_cost = algorithm == CycleCanceling ? 0. : 2 * maximum_edge_weight(gc.edges());
        ///Lambda function to add an edge in the boost graph (with its residual edge, weight and capacity).
        ///This way we avoid duplicate code. (capacity of the edge is set to 1 as default)
        auto add_edge = [&](ID tail, ID head, double cost, unsigned cap = 1) {
            auto const edge_descriptor = boost::add_edge(tail, head, g).first;
            auto const reverse_edge_descriptor = boost::add_edge(head, tail, g).first;
            rev[edge_descriptor] = reverse_edge_descriptor;
            rev[reverse_edge_descriptor] = edge_descriptor;
            capacity[edge_descriptor] = cap;
            capacity[reverse_edge_descriptor] = 0;
            weight[edge_descriptor] = static_cast<int>(cost);
            //Cost of the reverse edge must be inverted!
            weight[reverse_edge_descriptor] = -static_cast<int>(cost);
            residual_capacity[edge_descriptor] = cap;
            residual_capacity[reverse_edge_descriptor] = 0;
            edge_to_edge_descriptor[{tail, head}] = edge_descriptor;
            return edge_descriptor;
        };
        for (auto const &edge : gc.edges()) {
            auto const pupil_idx = edge.pupil_id;
            auto const college_student_idx = edge.college_student_id + gc.nodes().pupils().size();
            auto const edge_descriptor = add_edge(pupil_idx, college_student_idx, -edge.cost + additional_cost);
        }
        ///Add edges with 0 weight between s and pupils:
        for (auto const &pupil : gc.nodes().pupils()) {
            add_edge(gc.s_id(), pupil.id(), 0.);
        }
        ///Add edges with 0 weight between students and t:
        for (auto const &student : gc.nodes().college_students()) {
            auto const student_index = gc.nodes().pupils().size() + student.id();
            add_edge(student_index, gc.t_id(), 0.);
        }
        unsigned const successive_flow_value = std::min(gc.nodes().pupils().size(), gc.nodes().college_students().size());
        CostValue inverted_flow_cost = (algorithm  == CycleCanceling) ?
                compute_max_cost_matching_cycle_canceling(gc, edge_to_edge_descriptor, g, rev, residual_capacity, add_edge):
                compute_max_cost_matching_successive_shortest_paths(gc, g, add_edge, additional_cost) - successive_flow_value * additional_cost;

        ///Store the computed matching edges:
        for (auto const & edge: gc.edges()) {
            ID const college_student_id = edge.college_student_id + gc.nodes().pupils().size();
            if (residual_capacity[edge_to_edge_descriptor[{edge.pupil_id,college_student_id}]] == 0){
                matching_edges.push_back(edge);
            }
        }
        ///Invert the flow cost, because we computed a min cost flow for the negative costs.
        return  -inverted_flow_cost;
    }
}

#endif //CORONA_SCHOOL_MATCHING_MATCHING_H
