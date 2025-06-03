#include "load.h"
#include "structures.h"
#include "utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

double convert_size(size_t size, char *units)
{
    size_t size_limit = 1024;
    if (size < (size_limit)) {
        strcpy(units, "B  ");
        return size;
    }
    size_limit <<= 10;
    if (size < (size_limit)) {
        strcpy(units, "KiB");
        return size / (double) (1UL << 10);
    }
    size_limit <<= 10;
    if (size < (size_limit)) {
        strcpy(units, "MiB");
        return size / (double) (1UL << 20);
    }
    size_limit <<= 10;
    if (size < (size_limit)) {
        strcpy(units, "GiB");
        return size / (double) (1UL << 30);
    }
    size_limit <<= 10;
    if (size < (size_limit)) {
        strcpy(units, "TiB");
        return size / (double) (1UL << 40);
    }
    size_limit <<= 10;
    if (size < (size_limit)) {
        strcpy(units, "PiB");
        return size / (double) (1UL << 50);
    }
    return -1;
}

double calculate_percentage(double num, double whole)
{
    if (whole == 0) {
        return 100;
    }
    return (num / whole) * 100;
}

void print_node(struct node *node, struct tree_prefix *tree_prefix, struct switches *switches, bool errors_happened, size_t root_size)
{
    if (errors_happened) {
        printf("%s", node->error_flag ? "? " : "  ");
    }

    if (switches->percent_switch) {
        printf("%5.1lf%% ", calculate_percentage(node->size, root_size));
    } else {
        char units[4];
        printf("%6.1lf %s ", convert_size(node->size, units), units);
    }
    printf("%s%s\n", tree_prefix->string, node->name);
}

void change_tree_prefix(struct tree_prefix *tree_prefix)
{
    tree_prefix->string[strlen(tree_prefix->string) - 2] = ' ';
    tree_prefix->string[strlen(tree_prefix->string) - 3] = ' ';
    if (tree_prefix->string[strlen(tree_prefix->string) - 4] == '\\') {
        tree_prefix->string[strlen(tree_prefix->string) - 4] = ' ';
    }
}

void recursive_print(struct node *node, struct tree_prefix *tree_prefix, struct switches *switches, bool errors_happened, size_t depth, size_t root_size)
{
    if (switches->depth_switch && (depth > switches->max_depth)) {
        return;
    }
    print_node(node, tree_prefix, switches, errors_happened, root_size);

    if (strlen(tree_prefix->string) > 0) {
        change_tree_prefix(tree_prefix);
    }

    if (tree_prefix->capacity <= (strlen(tree_prefix->string) + tree_prefix->tree_level_length)) {
        tree_prefix->capacity *= 2;
        void *tmp = realloc(tree_prefix->string, tree_prefix->capacity);
        if (tmp == NULL) {
            fprintf(stderr, "Allocation failed.\n");
            return;
        }
        tree_prefix->string = tmp;
    }

    for (size_t i = 0; i < node->children_size; i++) {
        strcat(tree_prefix->string, (i == (node->children_size - 1) ? "\\-- " : "|-- "));
        recursive_print(&node->children[i], tree_prefix, switches, errors_happened, depth + 1, root_size);
        tree_prefix->string[strlen(tree_prefix->string) - tree_prefix->tree_level_length] = '\0';
    }
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Invalid number of arguments.\n");
        return 1;
    }

    struct switches switches;
    if (parse_switches(argc, argv, &switches) != 0) {
        return 1;
    }

    struct stat node_stats;
    if (stat(argv[argc - 1], &node_stats) != 0) {
        perror("Stat lookup failed");
        return 1;
    }

    if (!S_ISREG(node_stats.st_mode) && !S_ISDIR(node_stats.st_mode)) {
        return 0;
    }

    struct node root;
    bool alloc_fail = false;

    if (S_ISREG(node_stats.st_mode)) {
        init_node(&root);
        set_node_name(&root, argv[argc - 1], &alloc_fail);
        set_node_size(&root, &node_stats, &switches);
    }

    if (S_ISDIR(node_stats.st_mode)) {
        if (load_directory(&root, &node_stats, &switches, &alloc_fail, argc, argv) != 0) {
            return 1;
        }
    }

    struct tree_prefix tree_prefix;
    init_tree_prefix(&tree_prefix, &alloc_fail);
    if (alloc_fail) {
        destroy_tree(&root);
        return 1;
    }

    recursive_print(&root, &tree_prefix, &switches, root.error_flag, 0, root.size);
    free(tree_prefix.string);
    destroy_tree(&root);

    return 0;
}
