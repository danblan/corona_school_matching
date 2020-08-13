#include <iostream>
#include "graph_creator.h"
#include "nlohmann/json.hpp"
#include <algorithm>
using json = nlohmann::json;

namespace CS{
    //Helper method to convert a string encoding a state into the suiting enum.
    Bundesland  parse_bundesland(std::string bl) {
        ///Convert the string to lower case:
        std::for_each(bl.begin(), bl.end(), [](char & c){
            c = std::tolower(c);
        });
        if (bl == "hamburg") return Hamburg;
        if (bl == "berlin") return Berlin;
        if (bl == "brandenburg") return Brandenburg;
        if (bl == "bremen") return Bremen;
        if (bl == "rheinland-pfalz") return RheinlandPfalz;
        if (bl == "nordrhein-westfahlen") return NordrheinWestfalen;
        if (bl == "saarland") return Saarland;
        if (bl == "sachsen") return Sachsen;
        if (bl == "sachsen-anhalt") return SachsenAnhalt;
        if (bl == "bayern") return Bayern;
        if (bl == "baden-württemberg") return BadenWuerttemberg;
        if (bl == "hessen") return Hessen;
        if (bl == "niedersachsen") return Niedersachsen;
        if (bl == "thüringen") return Thueringen;
        if (bl == "schleswig-holstein") return SchleswigHolstein;
        if (bl == "mecklemburg-vorpommern") return MecklemburgVorpommern;
        return Bayern;
    }

    //Helper method converting a string encoding a subject into the suiting enum.
    Subject parse_subject(std::string s) {
        ///Convert the string to lower case:
        std::for_each(s.begin(), s.end(), [](char & c){
            c = std::tolower(c);
        });
        if (s == "mathematik") return Mathematik;
        if (s == "deutsch") return Deutsch;
        if (s == "englisch") return Englisch;
        if (s == "geschichte") return Geschichte;
        if (s == "chemie") return Chemie;
        if (s == "physik") return Physik;
        if (s == "biologie") return Biologie;
        if (s == "sozialkunde") return Sozialkunde;
        if (s == "spanisch") return Spanisch;
        if (s == "französisch") return Franzoesisch;
        if (s == "informatik") return Informatik;
        if (s == "erdkunde") return Erdkunde;
        if (s == "latein") return Latein;
        if (s == "wirtschaft") return Wirtschaft;
        if (s == "politik") return Politik;
        if (s == "philosophie") return Philosophie;
        if (s == "musik") return Musik;
        if (s == "kunst") return Kunst;
        if (s == "pädagogik") return Paedagogik;
        if (s == "religion") return Religion;
        if (s == "altgriechisch") return Altgriechisch;
        if (s == "italienisch") return Italienisch;
        if (s == "russisch") return Russisch;
        if (s == "niederländisch") return Niederlaendisch;
        if (s == "chinesisch") return Chinesisch;
        else std::cout<<"subject not identified, name was"<<s<<std::endl;
    }

    //TODO: Add some safety checks in this function
    void GraphCreator::init_from_json(std::ifstream & pupil_file, std::ifstream & student_file) {
        json pupil_data_file, student_data_file;
        //Initialize the json object:
        pupil_file >> pupil_data_file;
        student_file >> student_data_file;
        //Build pupils and students with the data in the json file.
        _nodes.create_pupils(pupil_data_file.size());
        unsigned pupil_count{0u};
        for (auto const & pupil_json_data : pupil_data_file) {
            auto & pupil_data = _nodes.pupil(pupil_count).data();
            ///bundesland not yet featured
            //pupil_data.bundesland = parse_bundesland(pupil_json_data["Bundesland"]);
            pupil_data.input_file_id = pupil_json_data["id"];
            pupil_data.grade = pupil_json_data["grade"];
            for (auto const & fach : pupil_json_data["subjects"]) {
                pupil_data.requested_subjects.emplace_back(parse_subject(fach["name"]));
            }
            pupil_count++;
        }
        _nodes.create_college_students(student_data_file.size());
        unsigned student_count{0u};
        for (auto const & student_json_data : student_data_file) {
            auto & student_data = _nodes.college_student(student_count).data();
            //student_data.bundesland = parse_bundesland(student_json_data["Bundesland"]);
            student_data.input_file_id = student_json_data["id"];
            for (auto const & offered_sub : student_json_data["subjects"]) {
                Subject const subject = parse_subject(offered_sub["name"]);
                GradeRange range;
                //Currently only with min and max
                unsigned min,max;
                if (offered_sub.find("grade") == std::end(offered_sub)) {
                    //Assume that in this case we have all grades
                    min = MIN_POSSIBLE_GRADE;
                    max = MAX_POSSIBLE_GRADE;
                } else {
                    min = offered_sub["grade"]["min"];
                    max = offered_sub["grade"]["max"];
                }
                for (unsigned grade = min; grade <= max; ++grade) {
                    range.grades.push_back(grade);
                }
                student_data.offered_subjects.push_back({subject, 1., range});
            }
            student_count++;
        }

        //Create edges:
        create_edges();
        //Cache the edge costs:
        init_edge_costs();
    }

    void GraphCreator::init_edge_costs() {
        //Initialize the cost components that are used currently:
        edge_cost_computer.add_bundesland_bonus();
        edge_cost_computer.add_fachuebereinstimmung();
        //Iterate over all edges and cache the costs which are computed by the edge cost computer.
        for (auto & edge : _edges){
            edge.cost = edge_cost_computer.compute_edge_cost(
                    _nodes.college_student(edge.college_student_id), _nodes.pupil(edge.pupil_id));
        }
    }

    void GraphCreator::create_edges() {
        for (auto const & pupil : _nodes.pupils()){
            for (auto const & student : _nodes.college_students()) {
                if (pupil.accepts(student) and student.accepts(pupil)) {
                    _edges.emplace_back(pupil.id(), student.id());
                }
            }
        }
    }
}


