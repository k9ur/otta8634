const { sortByDependencies } = require("./sort-by-dependencies.js");


A = { name: "A", before: ["B"] };
B = { name: "B" };
C = { name: "C", before: ["E"], after: ["B"] };
D = { name: "D", after: ["C"] };
E = { name: "E", after: ["A", "B"] };
F = {}; // has no name so can't be sorted

testArray = [C, D, F, E, A, B];
console.log(sortByDependencies(testArray));
// [ A, B, C, E, D ] or [ A, B, C, D, E ] are valid, with F placed anywhere, most likely towards the end
