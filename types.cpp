#include "types.h"

namespace  CS {
    CollegeStudent::CollegeStudent(ID id) : NodeIF(id) {
            _acceptance_function = [&] (const Pupil & pupil) {
                for (auto const & offered_subject : _data.offered_subjects) {
                    if (not offered_subject.grade_range.contains(pupil.data().grade)) {
                        //Grade does not fit for this subject!
                        continue;
                    }
                    for (auto const & requested_subject : pupil.data().requested_subjects) {
                        if (requested_subject.subject == offered_subject.subject) {
                            return true;
                        }
                    }
                }
                return false;
            };
    }
}
