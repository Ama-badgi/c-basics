#include "data_source.h"
#include "structures.h"

#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool is_decimal_number(const char *str)
{
    bool decimal_point_found = false;
    int decimal_digits_count = 0;

    for (size_t idx = (str[0] == '-') ? 1 : 0; idx < strlen(str); idx++) {
        if ((str[idx] == '.') && (!decimal_point_found)) {
            decimal_point_found = true;
            continue;
        }
        if ((str[idx] == '.') && (decimal_point_found)) {
            return false;
        }

        if (isdigit(str[idx]) == 0) {
            return false;
        }

        if (decimal_point_found) {
            decimal_digits_count++;
        }
    }

    if (decimal_digits_count > 15) {
        return false;
    }

    return true;
}

bool is_number(const char *str)
{
    for (size_t idx = 0; idx < strlen(str); idx++) {
        if (isdigit(str[idx]) == 0) {
            return false;
        }
    }

    return true;
}

int validate_id(const char *id)
{
    if (strlen(id) == 0) {
        return 1;
    }
    if (!is_number(id)) {
        return 2;
    }

    return 0;
}

int validate_id_uniqueness(unsigned int id, struct Container *containers, size_t size)
{
    for (size_t idx = 0; idx < size; idx++) {
        if (containers[idx].id == id) {
            return 1;
        }
    }
    return 0;
}

int validate_coordinates(const char *longitude, const char *latitude)
{
    if ((strlen(longitude) == 0) || (strlen(latitude) == 0)) {
        return 1;
    }
    if (!is_decimal_number(latitude) || (!is_decimal_number(longitude))) {
        return 2;
    }
    return 0;
}

int validate_waste_type(const char *type)
{
    if ((strcmp(type, "Plastics and Aluminium") == 0) || (strcmp(type, "Paper") == 0) || (strcmp(type, "Biodegradable waste") == 0) || (strcmp(type, "Clear glass") == 0) || (strcmp(type, "Colored glass") == 0) || (strcmp(type, "Textile") == 0)) {
        return 0;
    }
    return 1;
}

int validate_capacity(const char *capacity)
{
    if (strlen(capacity) == 0) {
        return 1;
    }
    if (!is_number(capacity)) {
        return 2;
    }
    return 0;
}

int validate_number(const char *number)
{
    return !is_number(number) ? 1 : 0;
}

int validate_public(const char *public)
{
    return ((strcmp(public, "Y") == 0) || (strcmp(public, "N") == 0)) ? 0 : 1;
}

enum WasteType encode_waste_type(const char *type)
{
    if (strcmp(type, "Plastics and Aluminium") == 0) {
        return A;
    }
    if (strcmp(type, "Paper") == 0) {
        return P;
    }
    if (strcmp(type, "Biodegradable waste") == 0) {
        return B;
    }
    if (strcmp(type, "Clear glass") == 0) {
        return G;
    }
    if (strcmp(type, "Colored glass") == 0) {
        return C;
    }
    return T;
}

int validate_and_save_containers(struct Container **containers, size_t *containers_size)
{
    size_t line_index = 0;
    void *tmp;

    while (get_container_id(line_index) != NULL) {
        if (line_index >= *containers_size) {
            *containers_size *= 2;
            tmp = realloc(*containers, sizeof(struct Container) * (*containers_size + 1));
            if (tmp == NULL) {
                *containers_size = line_index;
                fprintf(stderr, "Reallocation function failed.\n");
                return 10;
            }
            *containers = tmp;
        }

        if (validate_id(get_container_id(line_index)) != 0) {
            *containers_size = line_index;
            fprintf(stderr, "Invalid id.\n");
            return 1;
        }
        unsigned long curr_id = strtoul(get_container_id(line_index), NULL, 10);
        if (curr_id > (UINT_MAX - 1)) {
            *containers_size = line_index;
            fprintf(stderr, "Id out of bounds.\n");
            return 1;
        }

        if (validate_id_uniqueness(curr_id, *containers, line_index) != 0) {
            *containers_size = line_index;
            fprintf(stderr, "Duplicate container id.\n");
            return 1;
        }
        (*containers)[line_index].id = (unsigned int) curr_id;

        if (validate_coordinates(get_container_x(line_index),
                    get_container_y(line_index)) != 0) {
            *containers_size = line_index;
            fprintf(stderr, "Invalid coordinates.\n");
            return 2;
        }
        (*containers)[line_index].longitude = strtod(get_container_x(line_index), NULL);
        (*containers)[line_index].latitude = strtod(get_container_y(line_index), NULL);

        if (validate_waste_type(get_container_waste_type(line_index)) != 0) {
            *containers_size = line_index;
            fprintf(stderr, "Invalid waste type.\n");
            return 3;
        }
        (*containers)[line_index].waste_type = encode_waste_type(get_container_waste_type(line_index));

        if (validate_capacity(get_container_capacity(line_index)) != 0) {
            *containers_size = line_index;
            fprintf(stderr, "Invalid capacity.\n");
            return 4;
        }
        unsigned long curr_capacity = strtoul(get_container_capacity(line_index), NULL, 10);
        if (curr_capacity > (UINT_MAX - 1)) {
            *containers_size = line_index;
            fprintf(stderr, "Capacity out of bounds.\n");
            return 4;
        }
        (*containers)[line_index].capacity = (unsigned int) curr_capacity;

        if (validate_number(get_container_number(line_index)) != 0) {
            *containers_size = line_index;
            fprintf(stderr, "Invalid number.\n");
            return 5;
        }
        unsigned long curr_number = strtoul(get_container_number(line_index), NULL, 10);
        if (curr_number > (UINT_MAX - 1)) {
            *containers_size = line_index;
            fprintf(stderr, "Container number out of bounds.\n");
            return 5;
        }
        (*containers)[line_index].address = NULL;
        if ((strlen(get_container_street(line_index)) > 0) && (strlen(get_container_number(line_index)) > 0)) {
            const char *street = get_container_street(line_index);
            const char *number = get_container_number(line_index);

            (*containers)[line_index].address = malloc(strlen(street) + strlen(number) + 2);
            if ((*containers)[line_index].address == NULL) {
                fprintf(stderr, "Allocation failed.\n");
                return 50;
            }
            strcpy((*containers)[line_index].address, street);
            strcat((*containers)[line_index].address, " ");
            strcat((*containers)[line_index].address, number);
        }

        if (validate_public(get_container_public(line_index)) != 0) {
            *containers_size = line_index;
            fprintf(stderr, "Invalid public state.\n");
            return 6;
        }
        (*containers)[line_index].public = get_container_public(line_index)[0];

        (*containers)[line_index].area_id = 0;

        line_index++;
    }

    *containers_size = line_index;

    return 0;
}

int find_id(unsigned int id, struct Container **containers, size_t *containers_size)
{
    for (size_t i = 0; i < *containers_size; i++) {
        if ((*containers)[i].id == id) {
            return 0;
        }
    }
    return 1;
}

int check_path_uniqueness(unsigned int container_a, unsigned int container_b, struct Path **paths, size_t paths_length)
{
    for (size_t i = 0; i < paths_length; i++) {
        if (((container_a == (*paths)[i].start) && (container_b == (*paths)[i].dest)) || ((container_b == (*paths)[i].start) && (container_a == (*paths)[i].dest))) {
            return 1;
        }
    }
    return 0;
}

int validate_and_save_paths(struct Path **paths, size_t *paths_size, struct Container **containers, size_t *containers_size)
{
    size_t line_index = 0;
    size_t paths_index = 0;
    void *tmp;

    while (get_path_a_id(line_index) != NULL) {
        if (paths_index >= *paths_size) {
            *paths_size *= 2;
            tmp = realloc(*paths, sizeof(struct Path) * (*paths_size + 1));
            if (tmp == NULL) {
                fprintf(stderr, "Reallocation function failed.\n");
                return 10;
            }
            *paths = tmp;
        }
        if ((validate_id(get_path_a_id(line_index)) != 0) || (validate_id(get_path_b_id(line_index)) != 0)) {
            fprintf(stderr, "Invalid id in paths.\n");
            return 1;
        }
        unsigned long container_a = strtoul(get_path_a_id(line_index), NULL, 10);
        unsigned long container_b = strtoul(get_path_b_id(line_index), NULL, 10);
        if ((container_a > (UINT_MAX - 1)) || (container_b > (UINT_MAX - 1))) {
            fprintf(stderr, "Path id out of bounds.\n");
            return 2;
        }
        (*paths)[paths_index].start = (unsigned int) container_a;
        (*paths)[paths_index].dest = (unsigned int) container_b;
        if ((find_id((*paths)[paths_index].start, containers, containers_size) != 0) || (find_id((*paths)[paths_index].dest, containers, containers_size) != 0)) {
            fprintf(stderr, "Id in paths not an id of a container.\n");
            return 3;
        }

        if (check_path_uniqueness((*paths)[paths_index].start, (*paths)[paths_index].dest, paths, paths_index) != 0) {
            line_index++;
            continue;
        }
        unsigned long distance = strtoul(get_path_distance(line_index), NULL, 10);
        if ((distance > (UINT_MAX - 1) || (distance <= 0))) {
            fprintf(stderr, "Path distance out of bounds.\n");
            return 4;
        }
        (*paths)[paths_index].distance = (unsigned int) distance;

        paths_index++;
        line_index++;
    }

    *paths_size = paths_index;
    return 0;
}

int validate_data(const char *containers_path, const char *paths_path, struct Container **containers, size_t *containers_size, struct Path **paths, size_t *paths_size)
{
    if (!init_data_source(containers_path, paths_path)) {
        fprintf(stderr, "Initializing function failed.\n");
        return 1;
    }
    if (validate_and_save_containers(containers, containers_size) != 0) {
        return 2;
    }
    if (validate_and_save_paths(paths, paths_size, containers, containers_size) != 0) {
        return 3;
    }

    destroy_data_source();
    return 0;
}

int validate_types(char *types)
{
    for (size_t i = 0; i < strlen(types); i++) {
        if ((types[i] != 'A') && (types[i] != 'P') && (types[i] != 'B') && (types[i] != 'G') && (types[i] != 'C') && (types[i] != 'T')) {
            fprintf(stderr, "Invalid waste type.\n");
            return 1;
        }
    }

    return 0;
}

bool is_unique_type(struct Filter *filter, size_t size, enum WasteType type)
{
    for (size_t i = 0; i < size; i++) {
        if ((filter->types)[i] == type) {
            return false;
        }
    }
    return true;
}

enum WasteType char_to_type(char type)
{
    switch (type) {
        case 'A':
            return A;
        case 'P':
            return P;
        case 'B':
            return B;
        case 'G':
            return G;
        case 'C':
            return C;
        default:
            return T;

    }
}

void save_types(char *arguments, struct Filter *filter) {
    size_t idx = 0;
    filter->type_f = true;

    for (size_t i = 0; i < strlen(arguments); i++) {
        if (is_unique_type(filter, idx, arguments[i])) {
            filter->types[idx] = char_to_type(arguments[i]);
            idx++;
        }
    }
    filter->types_size = idx;
}

int validate_capacity_range(char **range, size_t *snd_number_idx)
{
    bool hyphen_found = false;
    size_t hyphen_idx;

    if (isdigit((*range)[0]) == 0) {
        fprintf(stderr, "First number missing in range.\n");
        return 1;
    }

    for (size_t i = 0; i < strlen(*range); i++) {
        if (hyphen_found && ((*range)[i] == '-')) {
            fprintf(stderr, "Duplicate hyphen found in filter range.\n");
            return 2;
        }

        if (!hyphen_found && ((*range)[i] == '-')) {
            hyphen_found = true;
            hyphen_idx = i;
            if ((strlen(*range) == hyphen_idx + 1) || (isdigit((*range)[hyphen_idx + 1]) == 0)) {
                fprintf(stderr, "Second number missing in filter range.\n");
                return 3;
            }
            (*range)[hyphen_idx] = '\0';
            continue;
        }

        if (isdigit((*range)[i]) == 0) {
            fprintf(stderr, "Invalid number in filter range.\n");
            return 4;
        }
    }

    if (!hyphen_found) {
        fprintf(stderr, "No hyphen found in the range.\n");
        return 5;
    }

    unsigned long low = strtoul(*range, NULL, 10);
    unsigned long high = strtoul((*(range) + hyphen_idx + 1), NULL, 10);
    if ((low > (UINT_MAX - 1)) || (high > (UINT_MAX - 1))) {
        fprintf(stderr, "Number in filter range out of bounds.\n");
        return 6;
    }

    if (low > high) {
        fprintf(stderr, "Left number greater than right number.\n");
        return 7;
    }

    *snd_number_idx = hyphen_idx + 1;

    return 0;
}

void save_capacity(char *arguments, size_t snd_number_idx, struct Filter *filter)
{
    filter->capacity_f = true;
    filter->low = (unsigned int) strtoul(arguments, NULL, 10);
    filter->high = (unsigned int) strtoul((arguments + snd_number_idx), NULL, 10);
}

void save_public(char *arguments, struct Filter *filter)
{
    filter->public_f = true;

    filter->public = arguments[0];
}

int validate_filter(char *filter_switch, char **arguments, struct Filter *filter)
{
    if ((strcmp("-t", filter_switch) == 0) && (!filter->type_f)) {
        if (validate_types(*arguments) != 0) {
            return 1;
        }
        save_types(*arguments, filter);
        return 0;
    }

    if ((strcmp(filter_switch, "-c") == 0) && (!filter->capacity_f)) {
        size_t snd_number_idx;
        if (validate_capacity_range(arguments, &snd_number_idx) != 0) {
            return 2;
        }
        save_capacity(*arguments, snd_number_idx, filter);
        return 0;
    }

    if ((strcmp(filter_switch, "-p") == 0) && (!filter->public_f)) {
        if (validate_public(*arguments) != 0) {
            fprintf(stderr, "Invalid public filter argument.\n");
            return 3;
        }
        save_public(*arguments, filter);
        return 0;
    }

    fprintf(stderr, "Invalid filter switch.\n");
    return 10;
}

int parse_filter_switches(int argc, char **argv, struct Filter *filter)
{
    for (int i = 1; i < (argc - 2); i += 2) {
        if (validate_filter(argv[i], &argv[i + 1], filter) != 0) {
            return 1;
        }
    }
    return 0;
}
