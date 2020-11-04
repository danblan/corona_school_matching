#ifndef CORONA_SCHOOL_MATCHING_GRAPH_CREATOR_H
#define CORONA_SCHOOL_MATCHING_GRAPH_CREATOR_H

#include "types.h"
#include "costs.h"
#include <fstream>

namespace CS {

    /**
     * Simple struct for edges between pupils and students.
     */
    struct Edge {
        ID  pupil_id;
        ID  college_student_id;
        CostValue cost = 0.;
        Edge(ID p, ID c) : pupil_id{p}, college_student_id{c} {}
    };

    class GraphCreator {
    public:
        /**
         * Main function of the Graph Creator.
         * @param pupil_file A json file which describes the pupils of the instance
         * @param student_file A json file which described the students of the instance
         * @param balancing_coefficients A json file that describes the fraction of the total cost that
         * each cost component should contribute.
         * Given the json files this function constructs all students and pupils and
         * edges between them. Further, costs are initialized and balanced.
         * @note The coefficients in the last file should all be between 0 and 1 and add up to 1.
         */
        void
        init_from_json(std::ifstream &pupil_file, std::ifstream &student_file, std::ifstream &balancing_coefficients,
                std::optional<std::vector<std::string>> const & pupil_uuids, std::optional<std::vector<std::string>> const & student_uuids);

        /**
         * @return The id of the virtual source node s in the min cost flow instance.
         */
        [[nodiscard]] inline ID s_id() const { return _nodes.size(); }

        /**
         * @return The id of the virtual source node t in the min cost flow instance.
         */
        [[nodiscard]] inline ID t_id() const { return _nodes.size() + 1; }

        /**
         * @return The total numbe of nodes, that is number of students + number of pupils + 2 virtual nodes (s and t)
         */
        [[nodiscard]] inline std::uint32_t num_nodes() const { return _nodes.size() +2; }

        /**
         * @return A const reference to the container of edges.
         */
        [[nodiscard]] inline std::vector<Edge> const & edges() const {return _edges;}

        /**
         * @return A const reference to the container of nodes.
         */
        [[nodiscard]] inline NodeContainer const & nodes() const {return _nodes;}

        /**
         * @return A const reference to the cost computer.
         */
        [[nodiscard]] inline EdgeCostComputer const & cost_computer() const {return edge_cost_computer;}


    private:

        /**
         * @brief: Creates the edges between students and pupils and stores them.
         */
        void create_edges();

        /**
         * @brief: Initializes the costs of all edges between students and pupils.
         */
        void init_edge_costs();

        /**
         * This function adapts the edge costs in order to achieve given coefficients.
         * E.g. if the input file specifies that 90% of the total costs should be reserved for matching subjects,
         * then the coefficients of the edge cost computer will be adapted accordingly.
         * @param balancing_coefficients Input file stream which specifies the fraction of the total
         * that should be achieved by every cost component.
         */
        void balance_edge_costs(std::ifstream & balancing_coefficients);

        //Container for nodes and edges
        std::vector<Edge> _edges;
        NodeContainer _nodes;
        EdgeCostComputer edge_cost_computer;
    };
}


#endif //CORONA_SCHOOL_MATCHING_GRAPH_CREATOR_H
