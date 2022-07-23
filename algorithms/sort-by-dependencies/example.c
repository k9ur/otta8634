#include "sort-by-dependencies.h"

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
		for(short i = 0; i < array_size; i++)
			printf("%s, ", output_arr[i]->name);
		printf("\n");
		return 0; // [ A, B, C, (any order of D,E,F) ] is valid
	}
	
	char* output_str = (char *) output;
	printf("%s\n", output_str);
	return 1;
}
