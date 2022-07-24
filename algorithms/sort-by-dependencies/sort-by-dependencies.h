#ifndef SORT_BY_DEPEN_H
#define SORT_BY_DEPEN_H

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

extern uint8_t sort_success;
extern size_t array_size;

void* sort_by_dependencies(Object *[], short);

#endif
