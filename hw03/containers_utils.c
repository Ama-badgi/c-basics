#include "structures.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compare(const void *x, const void *y)
{
    unsigned int *first = (unsigned int *) x;
    unsigned int *second = (unsigned int *) y;
    return *first - *second;
}

int save_neighbours(struct Container **containers, size_t containers_size, struct Path **paths, size_t paths_size)
{
    for (size_t container_idx = 0; container_idx < containers_size; container_idx++) {
        (*containers)[container_idx].neighbours_size = 0;
        (*containers)[container_idx].neighbours = NULL;
        for (size_t path_idx = 0; path_idx < paths_size; path_idx++) {
            if (((*containers)[container_idx].id == (*paths)[path_idx].start) || ((*containers)[container_idx].id == (*paths)[path_idx].dest)) {
                (*containers)[container_idx].neighbours_size++;
            }
        }
    }

    for (size_t container_idx = 0; container_idx < containers_size; container_idx++) {
        if ((*containers)[container_idx].neighbours_size == 0) {
            continue;
        }

        size_t neighbours_idx = 0;
        (*containers)[container_idx].neighbours = malloc(sizeof(unsigned int) * (*containers)[container_idx].neighbours_size);
        if ((*containers)[container_idx].neighbours == NULL) {
            fprintf(stderr, "Allocation failed.\n");
            return 1;
        }
        for (size_t path_idx = 0; path_idx < paths_size; path_idx++) {
            if ((*containers)[container_idx].id == (*paths)[path_idx].start) {
                (*containers)[container_idx].neighbours[neighbours_idx] = (*paths)[path_idx].dest;
                neighbours_idx++;
            }

            if ((*containers)[container_idx].id == (*paths)[path_idx].dest) {
                (*containers)[container_idx].neighbours[neighbours_idx] = (*paths)[path_idx].start;
                neighbours_idx++;
            }
        }

        qsort((*containers)[container_idx].neighbours, (*containers)[container_idx].neighbours_size, sizeof(unsigned int), compare);
    }
    return 0;
}

const char *type_to_desc(char type)
{
    switch (type) {
    case A:
        return "Plastics and Aluminium";
    case P:
        return "Paper";
    case B:
        return "Biodegradable waste";
    case G:
        return "Clear glass";
    case C:
        return "Colored glass";
    case T:
        return "Textile";
    }
    return NULL;
}

char type_to_char(enum WasteType type)
{
    switch (type) {
        case A:
            return 'A';
        case P:
            return 'P';
        case B:
            return 'B';
        case G:
            return 'G';
        case C:
            return 'C';
        default:
            return 'T';
    }
}

void print_container(struct Container container)
{
    printf("ID: %u, Type: %s, Capacity: %u, ",
           container.id,
           type_to_desc(container.waste_type),
           container.capacity);
    if (container.address == NULL) {
        printf("Address:, ");
    } else {
        printf("Address: %s, ", container.address);
    }

    printf("Neighbors:");
    for (size_t neigh_idx = 0; neigh_idx < container.neighbours_size; neigh_idx++) {
        printf(" %u", container.neighbours[neigh_idx]);
    }
    printf("\n");
}

void print_all_containers(struct Container *containers, size_t containers_size)
{
    for (size_t cont_idx = 0; cont_idx < containers_size; cont_idx++) {
        print_container(containers[cont_idx]);
    }
}

bool is_correct_type(struct Container container, struct Filter filter)
{
    if (!filter.type_f) {
        return true;
    }
    for (size_t i = 0; i < filter.types_size; i++) {
        if (container.waste_type == filter.types[i]) {
            return true;
        }
    }
    return false;
}

bool is_correct_capacity(struct Container container, struct Filter filter)
{
    if (filter.capacity_f) {
        return ((container.capacity >= filter.low) && (container.capacity <= filter.high));
    }
    return true;
}

bool is_correct_public(struct Container container, struct Filter filter)
{
    return filter.public_f ? (container.public == filter.public) : true;
}

void filter_containers(struct Container **containers, size_t containers_size, struct Filter filter)
{
    for (size_t idx = 0; idx < containers_size; idx++) {
        if (is_correct_type((*containers)[idx], filter) && is_correct_capacity((*containers)[idx], filter) &&
                is_correct_public((*containers)[idx], filter)) {
            print_container((*containers)[idx]);
        }
    }
}

void make_container_areas(struct Container **containers, size_t *containers_size, size_t *areas_amount)
{
    unsigned long const COORDINATES_SHIFT = 100*1000UL*1000UL*1000UL*1000UL;
    unsigned int curr_area_id = 1;

    for (size_t idx = 0; idx < *containers_size; idx++) {
        if ((*containers)[idx].area_id != 0) {
            continue;
        }

        (*containers)[idx].area_id = curr_area_id;

        for (size_t cont_idx = 0; cont_idx < *containers_size; cont_idx++) {
            if ((*containers)[cont_idx].area_id != 0) {
                continue;
            }

            long cmp_lat = (long) (((*containers)[idx].latitude - (*containers)[cont_idx].latitude) * COORDINATES_SHIFT);
            long cmp_lon = (long) (((*containers)[idx].longitude - (*containers)[cont_idx].longitude) * COORDINATES_SHIFT);

            if ((cmp_lat == 0) && (cmp_lon == 0)) {
                (*containers)[cont_idx].area_id = curr_area_id;
            }
        }

        curr_area_id++;
    }

    *areas_amount = curr_area_id;
}

bool is_type_present(enum WasteType *types, enum WasteType type, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        if (types[i] == type) {
            return true;
        }
    }
    return false;
}

int type_compare(const void *a, const void *b)
{
    enum WasteType *fst = (enum WasteType *) a;
    enum WasteType *snd = (enum WasteType *) b;
    return *fst - *snd;
}

void print_area_waste_types(unsigned int area,
        struct Container **containers,
        size_t containers_size)
{
    enum WasteType type_switches[6];
    size_t idx = 0;

    for (size_t i = 0; i < containers_size; i++) {
        if ((*containers)[i].area_id != area) {
            continue;
        }
        if (!is_type_present(type_switches, (*containers)[i].waste_type, idx)) {
            type_switches[idx] = (*containers)[i].waste_type;
            idx++;
        }
    }

    qsort(type_switches, idx, sizeof(enum WasteType), type_compare);

    for (size_t i = 0; i < idx; i++) {
        printf("%c", type_to_char(type_switches[i]));
    }
    printf(";");
}

bool check_area_id_uniqueness(unsigned int area,
        unsigned int **area_neighbours,
        size_t size)
{
    for (size_t idx = 0; idx < size; idx++) {
        if ((*area_neighbours)[idx] == area) {
            return false;
        }
    }
    return true;
}

void print_area_neighbours(unsigned int *neighbours, int size)
{
    for (int idx = 0; idx < (size - 1); idx++) {
        printf("%u,", neighbours[idx]);
    }
    if (size > 0) {
        printf("%u", neighbours[size - 1]);
    }
}

int find_container_area_neighbours(struct Container *curr_container, struct Container **containers, size_t containers_size,
        unsigned int curr_neighbour, unsigned int **area_neighbours, int *area_neigh_idx, int *area_neigh_size)
{
    void *tmp;

    for (size_t idx = 0; idx < containers_size; idx++) {
        if ((*containers)[idx].id != curr_neighbour) {
            continue;
        }

        if ((*containers)[idx].area_id != curr_container->area_id) {
            if (*area_neigh_idx >= *area_neigh_size) {
                *area_neigh_size *= 2;
                tmp = realloc(*area_neighbours, sizeof(unsigned int) * (*area_neigh_size + 1));
                if (tmp == NULL) {
                    fprintf(stderr, "Reallocation failed.\n");
                    free(*area_neighbours);
                    return 2;
                }
                *area_neighbours = tmp;
            }

            if (check_area_id_uniqueness((*containers)[idx].area_id,
                                         area_neighbours,
                                         *area_neigh_idx)) {
                (*area_neighbours)[*area_neigh_idx] = (*containers)[idx].area_id;
                (*area_neigh_idx)++;
            }
        }
    }
    return 0;
}

int make_area_neighbours(unsigned int area,
        struct Container **containers,
        size_t containers_size)
{
    int area_neigh_idx = 0;
    int area_neigh_size = 16;

    unsigned int *area_neighbours = malloc(sizeof(unsigned int) * (area_neigh_size + 1));
    if (area_neighbours == NULL) {
        fprintf(stderr, "Allocation failed.\n");
        return 1;
    }

    for (size_t i = 0; i < containers_size; i++) {
        struct Container curr_container = (*containers)[i];
        if (curr_container.area_id != area) {
            continue;
        }

        for (size_t neigh_idx = 0; neigh_idx < curr_container.neighbours_size; neigh_idx++) {
            unsigned int curr_neighbour = curr_container.neighbours[neigh_idx];
            if (find_container_area_neighbours(&curr_container, containers, containers_size, curr_neighbour,
                                               &area_neighbours, &area_neigh_idx, &area_neigh_size) != 0) {
                return 1;
            }
        }
    }
    qsort(area_neighbours, area_neigh_idx, sizeof(unsigned int), compare);
    print_area_neighbours(area_neighbours, area_neigh_idx);

    free(area_neighbours);
    area_neighbours = NULL;
    return 0;
}

int command_print_container_areas(struct Container **containers, size_t *containers_size)
{
    size_t areas_amount;

    make_container_areas(containers, containers_size, &areas_amount);

    for (unsigned int area = 1; area < areas_amount; area++) {
        printf("%u;", area);
        print_area_waste_types(area, containers, *containers_size);
        if (make_area_neighbours(area, containers, *containers_size) != 0) {
            return 1;
        }
        printf("\n");
    }
    return 0;
}
