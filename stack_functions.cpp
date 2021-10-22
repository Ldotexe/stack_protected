#include <stdio.h>
#include <cmath>
#include "stack_functions.h"
#if defined (ZERO_LEVEL) || defined (FIRST_LEVEL) || defined (SECOND_LEVEL)

const char* stack_error_name[] = {
    "STACK_ERROR_OK",
    "STACK_ERROR_NO_POINTER",
    "STACK_ERROR_NO_MEMORY",
    "STACK_ERROR_CAPACITY_LESS_ZERO",
    "STACK_ERROR_CAPACITY_LESS_SIZE",
    "STACK_ERROR_SIZE_LESS_ZERO",
    "STACK_ERROR_DATA_POINTER",
    "STACK_ERROR_LEFT_SHIELD",
    "STACK_ERROR_RIGHT_SHIELD",
    "STACK_ERROR_DATA_LEFT_SHIELD",
    "STACK_ERROR_DATA_RIGHT_SHIELD",
    "STACK_ERROR_DATA_HASH",
    "STACK_ERROR_HASH",
    "STACK_ERROR_CONSTRUCTED",
    "STACK_ERROR_DESTRUCTED"
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
    if (stack->data == 0){
        return STACK_ERROR_DATA_POINTER;
    }
    #ifndef ZERO_LEVEL
        if (stack->left_shield != shield_value){
            return STACK_ERROR_LEFT_SHIELD;
        }
        if (stack->right_shield != shield_value){
            return STACK_ERROR_RIGHT_SHIELD;
        }
        if ( *(shield_t*)((char*)stack->data - sizeof(shield_t)) != shield_value){
            return STACK_ERROR_DATA_LEFT_SHIELD;
        }
        if ( *(shield_t*)((char*)stack->data + stack->size * sizeof(elemen_t)) != shield_value){
            return STACK_ERROR_DATA_RIGHT_SHIELD;
        }
    #endif // ZERO_LEVEL

    #ifdef SECOND_LEVEL
        hash_t rem_data_hash = stack->data_hash;
        stack->data_hash = 0;
        hash_t rem_hash = stack->hash;
        stack->hash = 0;
        update_hash(stack);
        if (stack->data_hash != rem_data_hash){
            return STACK_ERROR_DATA_HASH;
        }
        if (stack->hash != rem_hash){
            return STACK_ERROR_HASH;
        }
    #endif // SECOND_LEVEL
    return STACK_ERROR_OK;
}


stack_error_code stack_ctor (struct stack_t *stack, int capacity){
    if (stack == NULL){
        return STACK_ERROR_NO_POINTER;
    }
    if (capacity < 0){
        return STACK_ERROR_CAPACITY_LESS_ZERO;
    }
    if (stack->capacity > 0){
        return STACK_ERROR_CONSTRUCTED;
    }
    stack->data = (elemen_t*)calloc(capacity * sizeof(elemen_t), sizeof(elemen_t));
    if (stack->data == 0){
        return STACK_ERROR_NO_MEMORY;
    }
    stack->capacity = capacity;
    #ifndef ZERO_LEVEL
        stack->left_shield = shield_value;
        stack->right_shield = shield_value;
    #endif // ZERO_LEVEL
    stack->size = 0;
    #ifdef SECOND_LEVEL
        update_hash(stack);
    #endif // SECOND_LEVEL

    return STACK_ERROR_OK;
}

stack_error_code stack_dtor(struct stack_t *stack){
    if(stack == NULL){
        return STACK_ERROR_NO_POINTER;
    }
    if (stack->data == 0){
        return STACK_ERROR_DESTRUCTED;
    }
    free(stack->data);
    stack->data = 0;
    stack->capacity = -1;
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
    printf("%u %u\n", stack->data_hash, stack->hash);
    return STACK_ERROR_OK;
}

#endif
