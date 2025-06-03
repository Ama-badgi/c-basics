#ifndef CONTAINERS_UTILS
#define CONTAINERS_UTILS

#include "structures.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int save_neighbours(struct Container **containers, size_t containers_size, struct Path **paths, size_t paths_size);

void print_all_containers(struct Container *containers, size_t containers_size);

void filter_containers(struct Container **containers, size_t containers_size, struct Filter filter);

int command_print_container_areas(struct Container **containers, size_t *containers_size);

#endif
