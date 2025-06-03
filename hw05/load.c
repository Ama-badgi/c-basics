#include "structures.h"
#include "utils.h"

#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int parse_switches(int argc, char **argv, struct switches *switches)
{
    switches->block_switch = true;
    switches->sort_switch = false;
    switches->percent_switch = false;
    switches->depth_switch = false;
    for (int i = 1; i < argc - 1; i++) {
        if ((strcmp(argv[i], "-a") == 0) && (switches->block_switch)) {
            switches->block_switch = false;
            continue;
        }
        if ((strcmp(argv[i], "-s") == 0) && (!switches->sort_switch)) {
            switches->sort_switch = true;
            continue;
        }
        if ((strcmp(argv[i], "-p") == 0) && (!switches->percent_switch)) {
            switches->percent_switch = true;
            continue;
        }
        if ((strcmp(argv[i], "-d") == 0) && (!switches->depth_switch)) {
            i++;
            if (argc <= i) {
                fprintf(stderr, "Number needed after depth switch.\n");
                return 1;
            }

            char *endptr;
            switches->depth_switch = true;
            switches->max_depth = strtoul(argv[i], &endptr, 10);
            if ((argv[i][0] == '-') || (strlen(endptr) != 0)) {
                fprintf(stderr, "Invalid number given.\n");
                return 1;
            }
            continue;
        }

        fprintf(stderr, "Invalid switch.\n");
        return 2;
    }
    return 0;
}

int name_cmp(const void *a, const void *b)
{
    struct node *node1 = (struct node *) a;
    struct node *node2 = (struct node *) b;

    int result = strcasecmp(node1->name, node2->name);
    return result != 0 ? result : strcmp(node1->name, node2->name);
}

int size_cmp(const void *a, const void *b)
{
    struct node *node1 = (struct node *) a;
    struct node *node2 = (struct node *) b;

    if (node1->size > node2->size) {
        return -1;
    }
    if (node1->size < node2->size) {
        return 1;
    }
    return name_cmp(a, b);
}

bool load_tree(DIR *dir, struct node *node, char *path, struct switches *switches, bool *alloc_fail)
{
    bool open_or_stat_failed = false;
    node->children = malloc(node->children_capacity * sizeof(struct node));
    if (node->children == NULL) {
        fprintf(stderr, "Allocation failed.\n");
        *alloc_fail = true;
        return true;
    }

    struct stat subdir_stats;
    struct dirent *subdir = NULL;

    while ((subdir = readdir(dir)) != NULL) {
        if (node->children_size >= node->children_capacity) {
            node->children_capacity *= 2;
            void *tmp = realloc(node->children, node->children_capacity * sizeof(struct node));
            if (tmp == NULL) {
                free(node->children);
                node->error_flag = true;
                *alloc_fail = true;
                return true;
            }
            node->children = tmp;
        }

        init_node(&node->children[node->children_size]);

        strcat(path, "/");
        strcat(path, subdir->d_name);

        if (stat(path, &subdir_stats) != 0) {
            fprintf(stderr, "Stat lookup failed: %s\n", path);
            path[strlen(path) - strlen(subdir->d_name) - 1] = '\0';
            node->children[node->children_size].error_flag = true;
            node->error_flag = true;
            open_or_stat_failed = true;

            set_node_name(&node->children[node->children_size], subdir->d_name, alloc_fail);
            if (*alloc_fail) {
                return true;
            }
            node->children_size++;
            continue;
        }
        if ((!S_ISREG(subdir_stats.st_mode) && !S_ISDIR(subdir_stats.st_mode)) || (strcmp(subdir->d_name, ".") == 0) || (strcmp(subdir->d_name, "..") == 0)) {
            path[strlen(path) - strlen(subdir->d_name) - 1] = '\0';
            continue;
        }

        set_node_name(&node->children[node->children_size], subdir->d_name, alloc_fail);
        if (*alloc_fail) {
            return true;
        }
        set_node_size(&node->children[node->children_size], &subdir_stats, switches);

        if (S_ISREG(subdir_stats.st_mode)) {
            path[strlen(path) - strlen(subdir->d_name) - 1] = '\0';
            node->children_size++;
            continue;
        }
        DIR *next_dir = opendir(path);
        if (next_dir == NULL) {
            fprintf(stderr, "Could not open: %s\n", path);
            path[strlen(path) - strlen(subdir->d_name) - 1] = '\0';
            node->children[node->children_size].error_flag = true;
            node->error_flag = true;
            open_or_stat_failed = true;
            node->children_size++;
            continue;
        }
        if (load_tree(next_dir, &node->children[node->children_size], path, switches, alloc_fail)) {
            node->error_flag = true;
            open_or_stat_failed = true;
        }
        closedir(next_dir);
        if (*alloc_fail) {
            return true;
        }
        path[strlen(path) - strlen(subdir->d_name) - 1] = '\0';
        node->children_size++;
    }

    for (size_t i = 0; i < node->children_size; i++) {
        node->size += node->children[i].size;
    }

    if (switches->sort_switch) {
        qsort(node->children, node->children_size, sizeof(struct node), size_cmp);
    } else {
        qsort(node->children, node->children_size, sizeof(struct node), name_cmp);
    }

    return open_or_stat_failed;
}

int load_directory(struct node *root, struct stat *stats, struct switches *switches, bool *alloc_fail, int argc, char **argv)
{
    DIR *dir = NULL;
    if ((dir = opendir(argv[argc - 1])) == NULL) {
        perror("Could not open passed directory");
        return 1;
    }

    char *path = malloc(4096);
    if (path == NULL) {
        *alloc_fail = true;
        fprintf(stderr, "Alloc failed.\n");
        return 1;
    }
    strcpy(path, argv[argc - 1]);

    init_node(root);
    set_node_name(root, argv[argc - 1], alloc_fail);
    set_node_size(root, stats, switches);

    root->error_flag = load_tree(dir, root, path, switches, alloc_fail);
    if (*alloc_fail) {
        destroy_tree(root);
        free(path);
        return 1;
    }
    closedir(dir);
    free(path);
    return 0;
}
