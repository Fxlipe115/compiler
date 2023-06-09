#ifndef ASSEMBLY_GENERATOR_H
#define ASSEMBLY_GENERATOR_H

#include <stdio.h>
#include "list.h"

void generate_assembly(FILE* stream, list_t* tacs);

#endif