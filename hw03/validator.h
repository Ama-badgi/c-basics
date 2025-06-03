#ifndef VALIDATOR
#define VALIDATOR

#include "data_source.h"
#include "structures.h"

#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int validate_data(const char *containers_path, const char *paths_path, struct Container **containers, size_t *containers_size, struct Path **paths, size_t *paths_size);

int validate_filter(char *filter_switch, char **arguments, struct Filter *filter);
int parse_filter_switches(int argc, char **argv, struct Filter *filter);

#endif
