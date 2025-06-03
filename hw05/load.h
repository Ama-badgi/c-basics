#ifndef LOAD_H
#define LOAD_H

#include "structures.h"

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

int parse_switches(int argc, char **argv, struct switches *switches);

bool load_tree(DIR *dir, struct node *node, char *path, struct switches *switches, bool *alloc_fail);
int load_directory(struct node *root, struct stat *stats, struct switches *switches, bool *alloc_fail, int argc, char **argv);

#endif
