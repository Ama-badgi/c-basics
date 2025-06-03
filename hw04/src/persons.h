#include "utils.h"

#include <stdlib.h>

#ifndef PERSONS_H
#define PERSONS_H

struct person
{
    char *id;
    char *name;
    double amount;
};

struct persons
{
    struct person *persons;
    size_t size;
    size_t capacity;
};

int init_persons(struct persons *persons);
int add_person(struct persons *persons, const char *id, const char *name);
struct person *find_person(const struct persons *persons, const char *id);

#endif
