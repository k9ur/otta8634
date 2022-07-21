from sortByDependencies import sort_by_dependencies


A = { "name": "A", "before": ["B"] }
B = { "name": "B" }
C = { "name": "C", "before": ["E"], "after": ["B"] }
D = { "name": "D", "after": ["C"] }
E = { "name": "E", "after": ["A", "B"] }
F = {} #has no name so can't be sorted

test_list = [C, D, F, E, A, B]
print(sort_by_dependencies(test_list))
