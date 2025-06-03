#include "persons.h"

#include "errors.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

int cmp(const void *a, const void *b)
{
    struct person *person1 = (struct person *) a;
    struct person *person2 = (struct person *) b;

    return strcmp(person1->id, person2->id);
}

static int person_cmp(const void *_id, const void *_person)
{
    const char *id = (const char *) _id;
    const struct person *person = (const struct person *) _person;
    return strcmp(id, person->id);
}

struct person *find_person(const struct persons *persons, const char *id)
{
    return (struct person *) bsearch(id, (const void *) persons->persons, persons->size, sizeof(struct person), person_cmp);
}

int init_persons(struct persons *persons)
{
    persons->capacity = 16;
    persons->size = 0;
    persons->persons = (struct person *) malloc(sizeof(struct person) * persons->capacity);
    if (persons->persons == NULL) {
        return ALLOCATION_FAILED;
    }
    return SUCCESS;
}

int add_person(struct persons *persons, const char *id, const char *name)
{
    if (find_person(persons, id) != NULL) {
        return PERSON_ALREADY_PRESENT;
    }

    if (persons->capacity == persons->size) {
        persons->capacity *= 2;
        struct person *tmp;
        tmp = (struct person *) realloc(persons->persons, persons->capacity * sizeof(struct person));
        if (tmp == NULL) {
            return ALLOCATION_FAILED;
        }
        persons->persons = tmp;
    }

    char *new_id = copy_string(id);
    if (new_id == NULL) {
        return ALLOCATION_FAILED;
    }
    char *new_name = copy_string(name);
    if (new_name == NULL) {
        free(new_id);
        return ALLOCATION_FAILED;
    }
    persons->persons[persons->size].id = new_id;
    persons->persons[persons->size].name = new_name;
    persons->persons[persons->size].amount = 0;
    persons->size++;

    qsort(persons->persons, persons->size, sizeof(struct person), cmp);
    return SUCCESS;
}
