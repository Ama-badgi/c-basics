#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdbool.h>
#include <stdlib.h>

struct switches
{
    bool block_switch;
    bool sort_switch;
    bool percent_switch;
    bool depth_switch;

    unsigned long max_depth;
};

struct node
{
    char *name;
    size_t size;
    bool error_flag;

    struct node *children;
    size_t children_size;
    size_t children_capacity;
};

struct tree_prefix
{
    char *string;
    size_t capacity;
    short tree_level_length;
};

#endif
