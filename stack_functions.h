#include <cstdlib> // calloc
#include <ctype.h> // type

#ifndef STACK_FUNCTIONS_H_INCLUDED
#define STACK_FUNCTIONS_H_INCLUDED

enum security{
    ZERO_LEVEL,
    FIRST_LEVEL, // shield
    SECOND_LEVEL // shield and hash
};

#define SECOND_LEVEL

typedef double elemen_t;
typedef unsigned long long hash_t;
typedef unsigned long long shield_t;

enum stack_error_code{
    STACK_ERROR_OK,
    STACK_ERROR_NO_POINTER,
    STACK_ERROR_NO_MEMORY,
    STACK_ERROR_CAPACITY_LESS_ZERO,
    STACK_ERROR_CAPACITY_LESS_SIZE,
    STACK_ERROR_SIZE_LESS_ZERO,
    STACK_ERROR_DATA_POINTER,
    STACK_ERROR_LEFT_SHIELD,
    STACK_ERROR_RIGHT_SHIELD,
    STACK_ERROR_DATA_LEFT_SHIELD,
    STACK_ERROR_DATA_RIGHT_SHIELD,
    STACK_ERROR_DATA_HASH,
    STACK_ERROR_HASH,
    STACK_ERROR_CONSTRUCTED,
    STACK_ERROR_DESTRUCTED,
    STACK_ERROR_FILE
};

extern const char* stack_error_name[];

// Aleksei bad idea da
const shield_t shield_value = 0xA1ECCE1BAD1DEADA;

extern const elemen_t stack_poison;
extern const int reducing_capacity;

struct stack_t{
    #ifndef ZERO_LEVEL
        shield_t left_shield = shield_value;
    #endif // ZERO_LEVEL

    #ifdef SECOND_LEVEL
        hash_t hash;
        hash_t data_hash;
    #endif // SECOND_LEVEL

    elemen_t *data;
    int capacity = -1;
    int size;

    #ifndef ZERO_LEVEL
        shield_t right_shield = shield_value;
    #endif // ZERO_LEVEL
};

stack_error_code stack_error (struct stack_t *stack);
stack_error_code stack_ctor (struct stack_t *stack, int capacity);
stack_error_code stack_dtor (struct stack_t *stack);
stack_error_code stack_resize_more(struct stack_t *stack);
int optimal_resize_more(int capacity);
stack_error_code stack_resize_less(struct stack_t *stack);
stack_error_code stack_push (struct stack_t *stack, elemen_t value);
elemen_t stack_pop (struct stack_t *stack);
hash_t stack_hash (char* buffer, long long size);
stack_error_code update_hash (struct stack_t *stack);
int stack_dump (struct stack_t *stack, const char* file, const char* function, const int line);

#endif// STACK_FUNCTIONS_H_INCLUDED
