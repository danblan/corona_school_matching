const bindings = require('bindings')('matching.node');

// The C++ matching algorithm that takes 4 paths to csv files and 1 path to an output file
const matching = bindings.matching;

/**
 * This function will purely perform Corona School's matching algorithm. It is just a TypeScript/JavaScript wrapper for the C++ matching algorithm – it will just call the matching algorithm node-addon.
 * @param pupilsFile Path to a csv file with the pupil data in the format that the matching algorithm expects.
 * @param studentsUpperBoundFile Path to a csv file with the student data (with UPPER bounds of grade information) in the format that the matching algorithm expects.
 * @param studentsLowerBoundFile Path to a csv file with the student data (with LOWER bounds of grade information) in the format that the matching algorithm expects.
 * @param dissolvedMatchesFile Path to a csv file with the dissolved matches data in the format that the matching algorithm expects.
 * @param outputFile Path to a csv file to which the algorithm should write its output to. IMPORTANT: the parent directory must exist, such that the algorithm can really write the output – it will not create any intermediate directories.
 */
function performAlgorithmPure(pupilsFile: string, studentsUpperBoundFile: string, studentsLowerBoundFile: string, dissolvedMatchesFile: string, outputFile: string) {
    console.log("-------- Will perform matching... --------");
    matching(pupilsFile, studentsUpperBoundFile, studentsLowerBoundFile, dissolvedMatchesFile, outputFile);
    console.log("------------ Matching done... ------------\n\n\n");
}

export default performAlgorithmPure;