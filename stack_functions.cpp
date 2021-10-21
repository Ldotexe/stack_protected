#include <stdio.h>
#include <cmath>
#include "stack_functions.h"
//#if defined (ZERO_LEVEL) || defined (FIRST_LEVEL) || defined (SECOND_LEVEL)

const char* stack_error_name[] = {
    "STACK_ERROR_OK",
    "STACK_ERROR_NO_POINTER",
    "STACK_ERROR_NO_MEMORY",
    "STACK_ERROR_CAPACITY_LESS_ZERO",
    "STACK_ERROR_CAPACITY_LESS_SIZE",
    "STACK_ERROR_SIZE_LESS_ZERO"
};

const elemen_t stack_poison = NAN;


stack_error_code stack_error (struct stack_t *stack){
    if (!stack){
        return STACK_ERROR_NO_POINTER;
    }
    if (stack->size >= stack->capacity){
        return STACK_ERROR_CAPACITY_LESS_SIZE;
    }
    if (stack->capacity < 0){
        return STACK_ERROR_CAPACITY_LESS_ZERO;
    }
    if (stack->size < 0){
        return STACK_ERROR_SIZE_LESS_ZERO;
    }
    return STACK_ERROR_OK;
}


stack_error_code stack_ctor (struct stack_t *stack, int capacity){
    if (stack == NULL){
        return STACK_ERROR_NO_POINTER;
    }
    stack->data = (elemen_t*)calloc(capacity * sizeof(elemen_t), sizeof(elemen_t));
    stack->capacity = capacity;
    stack->left_shield = shield_value;
    stack->right_shield = shield_value;
    stack->size = 0;
    update_hash(stack);
    return STACK_ERROR_OK;
}

stack_error_code stack_hash (stack_t *stack){
    return STACK_ERROR_OK;
}

stack_error_code stack_dtor(struct stack_t *stack){
    if(stack == NULL){
        return STACK_ERROR_NO_POINTER;
    }
    free(stack->data);
    stack->size = -1;
    return STACK_ERROR_OK;
}

stack_error_code stack_resize(struct stack_t *stack, int new_capacity){
    printf("Resizing memory old: %d new: %d\n", stack->capacity, new_capacity);
    stack->data = (elemen_t*) realloc(stack->data, new_capacity * sizeof(elemen_t));
    stack->capacity = new_capacity;
    update_hash(stack);
    return STACK_ERROR_OK;
}

stack_error_code stack_push(struct stack_t *stack, int value){
    if(stack == NULL){
        return STACK_ERROR_NO_POINTER;
    }
    if (stack->capacity == 0){
        stack_resize(stack, 1);
    }
    if(stack->size == stack->capacity){
        stack_resize(stack, stack->capacity * 2);
    }
    stack->data[stack->size++] = value;
    update_hash(stack);
    return STACK_ERROR_OK;
}

int stack_pop(struct stack_t *stack){
    if (stack->size > 0){
        if((stack->size) * 4 <= stack->capacity){
            stack_resize(stack, stack->capacity / 2);
        }
    }
    stack->size--;
    update_hash(stack);
    return stack->data[stack->size];
}

hash_t stack_hash(char* buffer, long long size){
	const char shift = 5;
	unsigned long long answer = 0;
	char buf1 = 0;
	char buf2 = 0;
	char buf3 = 0;

	for (long long counter = 0; counter < size; counter++)
	{
		answer += buffer[counter];
		buf1 = answer >> (sizeof (unsigned long long) - shift);
		buf2 = *((char*)&answer + 7) & 0xEC;
		buf3 = *((char*)&answer + 7) | 0x31;
		answer = (answer << shift) + buf1 + buf2 + buf3;
	}

	return answer;
}

stack_error_code update_hash(struct stack_t *stack){
    if(!stack){
        return STACK_ERROR_NO_POINTER;
    }

    stack->data_hash = 0;
    stack->data_hash = stack_hash((char*) stack->data, stack->size * sizeof(elemen_t));
    stack->hash = 0;
    stack->hash = stack_hash((char*)stack, 2 * sizeof(shield_t) + 2 * sizeof(hash_t) + sizeof(elemen_t*) + 2 * sizeof(int) );
    printf("%ld %ld\n", stack->data_hash, stack->hash);
    return STACK_ERROR_OK;
}

//#endif
