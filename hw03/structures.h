#ifndef STRUCTURES
#define STRUCTURES

#include <stdbool.h>
#include <stdlib.h>

enum WasteType
{
    A = 0, P, B, G, C, T
};

struct Container
{
    unsigned int id;
    double latitude;
    double longitude;
    unsigned int capacity;
    enum WasteType waste_type;
    char *address;
    char public;
    unsigned int *neighbours;
    size_t neighbours_size;
    unsigned int area_id;
};

struct Path
{
    unsigned int start;
    unsigned int dest;
    unsigned int distance;
};

struct Filter
{
    bool type_f;
    enum WasteType types[6];
    size_t types_size;

    bool capacity_f;
    unsigned int low;
    unsigned int high;

    bool public_f;
    char public;
};

#endif
