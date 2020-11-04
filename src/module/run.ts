const bindings = require('bindings')('corona_school_matching.node');
const matching = bindings.matching;

/**
 * This function will purely perform Corona School's matching algorithm. It is just a TypeScript/JavaScript wrapper for the C++ matching algorithm – it will just call the matching algorithm node-addon.
 * @param pupilsFile Path to a json file with the pupil data in the format that the matching algorithm expects.
 * @param studentsFile Path to a json file with the student data in the format that the matching algorithm expects.
 * @param balancingCoefficientsFile Path to a json file with the cost balancing coefficients in the format that the matching algorithm expects.
 * @param matchingOutputFile Path to a json file to which the algorithm should write its computed matches to. IMPORTANT: the parent directory must exist, such that the algorithm can really write the output – it will not create any intermediate directories.
 * @param statsOutputFile Path to a json file to which the algorithm should write its output statistics to. IMPORTANT: the parent directory must exist, such that the algorithm can really write the output – it will not create any intermediate directories.
 */
function performAlgorithmPure(pupilsFile: string, studentsFile: string, balancingCoefficientsFile: string, matchingOutputFile: string, statsoutputFile: string) {
    console.log("-------- Will perform matching... --------");
    matching(pupilsFile, studentsFile, balancingCoefficientsFile, matchingOutputFile, statsoutputFile);
    console.log("------------ Matching done... ------------\n\n\n");
}

export default performAlgorithmPure;