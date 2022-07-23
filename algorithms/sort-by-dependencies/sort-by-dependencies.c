#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAX_OBJECTS	200
#define MAX_LEN		51

typedef struct Object {
	char	name[MAX_LEN];
	char	before[MAX_OBJECTS][MAX_LEN];
	char	after[MAX_OBJECTS][MAX_LEN];
	short	before_size;
	short	after_size;
} Object;

uint8_t sort_success = 1; // Formerly named loop
size_t array_size = 0;
char trace[MAX_OBJECTS][MAX_LEN];
short trace_count = 0;
Object EMPTY_OBJECT = { { '\0' } };
Object *res[MAX_OBJECTS];
char trace_res[(MAX_OBJECTS + 2)*MAX_LEN + MAX_OBJECTS*4 + 45];

// Initialises variables
void init(short arr_size) {
	sort_success = 1; // Formerly named loop
	array_size = arr_size;
	trace_count = 0;
	Object EMPTY_OBJECT = { { '\0' } };
	memset(res, '\0', sizeof res);
	memset(trace_res, '\0', sizeof trace_res);
	memset(trace, '\0', sizeof trace);
}

// Gets object from array, searching by name
Object* get_obj(char name[], Object *array[]) {
	for(short i = 0; i < array_size; i++)
		if(!strcmp(array[i]->name, name))
			return array[i];
	return &EMPTY_OBJECT;
}

// Manages dependencies by expanding and re-ordering the array
// If we have A.before = { "B" }, B.before = { "C" }, A.before will be expanded into { "B", "C" }.
// depen is an array of the names of the (original or newly added) dependencies of obj
uint8_t manage_dependencies(Object *obj, char depen[][MAX_LEN], short depen_size, Object *array[]) {
	if(obj->name[0] == '\0' || depen[0][0] == '\0' || !sort_success)
		return sort_success;

	for(short i = 0; i < depen_size; i++) {
		char name[MAX_LEN];
		strcpy(name, depen[i]);
		if(!strcmp(obj->name, name)) { // Reached an infinite loop
			strcpy(trace[trace_count++], name);
			return sort_success = 0;
		}

		// Get new dependencies (dependencies of the before dependencies from the previous iteration)
		Object *before_obj = get_obj(name, array);
		if(before_obj->name[0] == '\0')
			continue;

		// Find indexes
		short i1 = -1, i2 = -1;
		for(short a_i = 0; a_i < array_size; a_i++) {
			if(!strcmp(array[a_i]->name, obj->name))
				i1 = a_i;
			else if(!strcmp(array[a_i]->name, before_obj->name))
				i2 = a_i;
			if(i1 != -1 && i2 != -1)
				break;
		}
		if(i1 > i2 && i2 != -1) { // obj should be before before_obj but isn't
			Object *temp, *prev = obj;
			for(short a_i = i2; a_i <= i1; a_i++) {
				temp = array[a_i]; // Shuffle the objects up in the array
				array[a_i] = prev; // So that obj is right before before_obj
				prev = temp;
			}
		}

		char next_depen[MAX_OBJECTS][MAX_LEN] = { '\0' };

		// Set next_depen to before_obj.before as long as it's not already in obj.before
		short next_depen_size = 0;
		for(short bo_i = 0; bo_i < before_obj->before_size; bo_i++) {
			uint8_t in = 0;
			for(short b_i = 0; b_i < obj->before_size; b_i++) {
				if(!strcmp(obj->before[b_i], before_obj->before[bo_i])) {
					in = 1;
					break; // It's already in obj.before
				}
			}
			if(!in) // If it's not in obj.before, add it to next_depen
				strcpy(next_depen[next_depen_size++], before_obj->before[bo_i]);
		}
		if(!next_depen_size)
			continue;

		// Add next_depen contents to obj.before
		for(short nd_i = 0; nd_i < next_depen_size; nd_i++)
			strcpy(obj->before[obj->before_size++], next_depen[nd_i]);

		uint8_t ret = manage_dependencies(obj, next_depen, next_depen_size, array); // Iterate again with new dependencies
		if(!ret) {
			strcpy(trace[trace_count++], name); // Add to trace
			return 0;
		}
	}

	return 1;
}

// Fixing befores then fixing afters isn't correct logic
// Function turns afters into befores
// array is required for get_obj
void manage_afters_and_befores(Object *obj, Object *array[]) {
	if(obj->name[0] == '\0')
		return;
	for(short a_i = 0; a_i < obj->after_size; a_i++) {
		Object *before_obj = get_obj(obj->after[a_i], array);
		if(before_obj->name[0] == '\0')
			continue;

		uint8_t in = 0;
		for(short ba_i = 0; ba_i < before_obj->before_size + 1; ba_i++) {
			if(!strcmp(before_obj->before[ba_i], obj->name)) {
				in = 1;
				break; // If obj.name is already in before_obj.before, quit
			}
		}
		if(!in) // Add obj.name to before_obj.before
			strcpy(before_obj->before[before_obj->before_size++], obj->name);
	}
	memset(obj->after, '\0', sizeof obj->after); // Now not needed
	obj->after_size = 0;
}

// Trace of how an object was linked back to itself as a dependency
char* print_trace() {
	strcpy(trace_res, trace[0]);
	strcat(trace_res, " was linked back to itself as a dependency: ");
	strcat(trace_res, trace[0]);
	for(short t_i = 0; t_i < trace_count; t_i++) {
		strcat(trace_res, " -> ");
		strcat(trace_res, trace[trace_count - t_i - 1]);
	}
	return trace_res;
}


// Main function
// array is the array to sort

void* sort_by_dependencies(Object *array[], short arr_size) {
	init(arr_size);

	for(short i = 0; i < array_size; i++) {
		res[i] = array[i]; // Copy array, so order of original array isn't changed in output

		for(short a_i = 0; a_i < MAX_OBJECTS; a_i++) { // Calculate after_size
			if(res[i]->after[a_i][0] == '\0') {
				res[i]->after_size = a_i;
				break;
			}
		}
		for(short b_i = 0; b_i < MAX_OBJECTS; b_i++) { // Calculate before_size
			if(res[i]->before[b_i][0] == '\0') {
				res[i]->before_size = b_i;
				break;
			}
		}
	}
	// Can't merge these iterations
	for(short i = 0; i < array_size; i++)
		manage_afters_and_befores(res[i], res);
	for(short i = 0; i < array_size; i++)
		if(!manage_dependencies(res[i], res[i]->before, res[i]->before_size, res))
			break;
	// Objects without a name can't be sorted, and by default are shuffled towards the end
	// Same behaviour for objects with no connected dependencies

	if(!sort_success)
		return print_trace(trace);
	return res;
}

#if 0
int main(int argc, char **argv) {
	Object A;
	strcpy(A.name, "A");
	Object B;
	strcpy(B.name, "B");
	Object C;
	strcpy(C.name, "C");
	Object D;
	strcpy(D.name, "D");
	Object E;
	strcpy(E.name, "E");
	Object F;
	strcpy(F.name, "F");

	Object *arr[] = { &C, &D, &F, &E, &A, &B };
	short arr_size = sizeof arr / sizeof *arr;
	for(short i = 0; i < arr_size; i++) {
		memset(arr[i]->before, '\0', sizeof arr[i]->after);
		memset(arr[i]->after, '\0', sizeof arr[i]->after);
	}

	// Sample data
	strcpy(A.before[0], "B");
	//strcpy(A.after[0], "D"); // Uncomment to create infinite loop
	strcpy(C.before[0], "E");
	strcpy(C.after[0], "B");
	strcpy(D.after[0], "C");
	strcpy(E.after[0], "A");
	strcpy(E.after[1], "B");

	// Handle data
	void* output = sort_by_dependencies(arr, arr_size);
	if(sort_success) {
		Object **output_arr = (Object**)output;
		for(short i = 0; i < arr_size; i++)
			printf("%s, ", output_arr[i]->name);
		printf("\n");
		return 0; // [ A, B, C, (any order of D,E,F) ] is valid
	}
	
	char* output_str = (char *) output;
	printf("%s\n", output_str);
	return 1;
}
#endif
