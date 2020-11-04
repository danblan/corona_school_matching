#!/usr/bin/env node

import { exit } from "process";
import { matchingAlgo } from "../module";

const args = process.argv.slice(2);

//check arguments
if (args.length !== 5) {
    console.log(`This program expects 5 paths as input:
        1. path to JSON with pupil-data 
        2. path to JSON with student-data
        3. path to JSON with balancing coefficients
        4. path to JSON where matches should be written into 
        5. path to JSON where stats should be written into
    `);

    exit(1);
}

//get arguments
const pupil_input_file = args[0];
const student_input_file = args[1];
const balancing_coefficients = args[2];
const matching_output = args[3];
const stats_output = args[4];

//perform algorithm
try {
    matchingAlgo(pupil_input_file, student_input_file, balancing_coefficients, matching_output, stats_output);
    console.log(`Matching results written to ${matching_output}\n`);
}
catch (e) {
    console.log(`🚨 A problem occurred...\n${e}\n`);
    exit(1);
}