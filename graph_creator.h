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
         * Given the json files this function constructs all students and pupils and
         * edges between them. Further, costs are initialized.
         */
        void init_from_json(std::ifstream & pupil_file, std::ifstream & student_file);

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


    private:

        /**
         * @brief: Creates the edges between students and pupils and stores them.
         */
        void create_edges();

        /**
         * @brief: Initializes the costs of all edges between students and pupils.
         */
        void init_edge_costs();

        //Container for nodes and edges
        std::vector<Edge> _edges;
        NodeContainer _nodes;
        EdgeCostComputer edge_cost_computer;
    };
}


#endif //CORONA_SCHOOL_MATCHING_GRAPH_CREATOR_H
