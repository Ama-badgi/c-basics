#ifndef UTILS_H
#define UTILS_H

#include <sys/stat.h>

void init_tree_prefix(struct tree_prefix *tree_prefix, bool *alloc_fail);
void init_node(struct node *node);

void destroy_tree(struct node *node);

void set_node_name(struct node *node, char *name, bool *alloc_fail);
void set_node_size(struct node *node, struct stat *stats, struct switches *switches);

#endif
