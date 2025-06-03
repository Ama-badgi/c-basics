#include "structures.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void init_tree_prefix(struct tree_prefix *tree_prefix, bool *alloc_fail)
{
    tree_prefix->tree_level_length = 4;
    tree_prefix->capacity = 8 * tree_prefix->tree_level_length;
    tree_prefix->string = malloc(tree_prefix->capacity);
    if (tree_prefix->string == NULL) {
        fprintf(stderr, "Allocation failed.\n");
        *alloc_fail = true;
        return;
    }
    tree_prefix->string[0] = '\0';
}

void init_node(struct node *node)
{
    node->name = NULL;
    node->size = 0;
    node->error_flag = false;

    node->children = NULL;
    node->children_capacity = 16;
    node->children_size = 0;
}

void destroy_tree(struct node *node)
{
    free(node->name);
    for (size_t i = 0; i < node->children_size; i++) {
        destroy_tree(&node->children[i]);
    }
    free(node->children);
}

void set_node_name(struct node *node, char *name, bool *alloc_fail)
{
    node->name = malloc(strlen(name) + 1);
    if (node->name == NULL) {
        fprintf(stderr, "Allocation failed.\n");
        *alloc_fail = true;
        return;
    }
    strcpy(node->name, name);
}

void set_node_size(struct node *node, struct stat *stats, struct switches *switches)
{
    if (switches->block_switch) {
        node->size = stats->st_blocks * 512;
    } else {
        node->size = stats->st_size;
    }
}
