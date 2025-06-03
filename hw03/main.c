#include "containers_utils.h"
#include "data_source.h"
#include "structures.h"
#include "validator.h"

#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void destroy_structs(struct Container **containers, size_t containers_size, struct Path **paths, bool neighbours_initialised)
{
    for (size_t i = 0; i < containers_size; i++) {
        if ((*containers)[i].address != NULL) {
            free((*containers)[i].address);
        }

        if (neighbours_initialised && ((*containers)[i].neighbours != NULL)) {
            free((*containers)[i].neighbours);
        }
    }
    free(*containers);
    free(*paths);
}

int initialize_structs(struct Container **containers, struct Path **paths, size_t *containers_size, size_t *paths_size, char *containers_file, char *paths_file)
{
    FILE *containers_csv = fopen(containers_file, "r");
    if (containers_csv == NULL) {
        free(*containers);
        free(*paths);
        fprintf(stderr, "Containers file does not exist.\n");
        return 5;
    }
    fclose(containers_csv);

    FILE *paths_csv = fopen(paths_file, "r");
    if (paths_csv == NULL) {
        free(*containers);
        free(*paths);
        fprintf(stderr, "Paths file does not exist.\n");
        return 5;
    }
    fclose(paths_csv);

    int validate_code = validate_data(containers_file, paths_file, containers, containers_size, paths, paths_size);

    if (validate_code == 1) {
        free(*containers);
        free(*paths);
        return 1;
    }

    if (validate_code != 0) {
        destroy_data_source();
        destroy_structs(containers, *containers_size, paths, false);
        return 2;
    }
    if (save_neighbours(containers, *containers_size, paths, *paths_size) != 0) {
        destroy_structs(containers, *containers_size, paths, true);
        return 50;
    }
    return 0;
}

int print_mode(char **argv)
{
    size_t containers_size = 1;
    size_t paths_size = 1;
    struct Container *containers = malloc(sizeof(struct Container) * (containers_size + 1));
    struct Path *paths = malloc(sizeof(struct Path) * (paths_size + 1));

    if ((containers == NULL) || (paths == NULL)) {
        fprintf(stderr, "Allocation failed.\n");
        return 50;
    }

    if (initialize_structs(&containers, &paths, &containers_size, &paths_size, argv[1], argv[2]) != 0) {
        return 1;
    }

    print_all_containers(containers, containers_size);

    destroy_structs(&containers, containers_size, &paths, true);
    return 0;
}

int areas_mode(char **argv)
{
    size_t containers_size = 1;
    size_t paths_size = 1;
    struct Container *containers = malloc(sizeof(struct Container) * (containers_size + 1));
    struct Path *paths = malloc(sizeof(struct Path) * (paths_size + 1));

    if ((containers == NULL) || (paths == NULL)) {
        fprintf(stderr, "Allocation failed.\n");
        return 50;
    }

    if (strcmp(argv[1], "-s") != 0) {
        free(containers);
        free(paths);
        fprintf(stderr, "Invalid switch.\n");
        return 1;
    }

    if (initialize_structs(&containers, &paths, &containers_size, &paths_size, argv[2], argv[3]) != 0) {
        return 1;
    }

    if (command_print_container_areas(&containers, &containers_size) != 0) {
        return 2;
    }

    destroy_structs(&containers, containers_size, &paths, true);
    return 0;
}

int filter_mode(int argc, char **argv, struct Filter *filter)
{
    size_t containers_size = 1;
    size_t paths_size = 1;
    struct Container *containers = malloc(sizeof(struct Container) * (containers_size + 1));
    struct Path *paths = malloc(sizeof(struct Path) * (paths_size + 1));

    if ((containers == NULL) || (paths == NULL)) {
        fprintf(stderr, "Allocation failed.\n");
        return 50;
    }

    if (parse_filter_switches(argc, argv, filter) != 0) {
        free(containers);
        free(paths);
        return 1;
    }

    if (initialize_structs(&containers, &paths, &containers_size, &paths_size, argv[argc - 2], argv[argc - 1]) != 0) {
        free(containers);
        free(paths);
        return 1;
    }

    filter_containers(&containers, containers_size, *filter);

    destroy_structs(&containers, containers_size, &paths, true);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Invalid launch parameters.\n");
    }

    if (argc == 3) {
        if (print_mode(argv) != 0) {
            return 1;
        }
        return 0;
    }
    if (argc == 4) {
        if (areas_mode(argv) != 0) {
            return 1;
        }
        return 0;
    }

    struct Filter filter;
    filter.type_f = false;
    filter.capacity_f = false;
    filter.public_f = false;

    return filter_mode(argc, argv, &filter);
}
