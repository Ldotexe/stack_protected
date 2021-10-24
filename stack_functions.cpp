#include <stdio.h>
#include <cmath>
#include <typeinfo>
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
    "STACK_ERROR_DESTRUCTED",
    "STACK_ERROR_FILE"
};

const elemen_t stack_poison = -666.200066666;
const int reducing_capacity = 2;

stack_error_code stack_error (struct stack_t *stack){
    if (!stack){
        return STACK_ERROR_NO_POINTER;
    }
    if (stack->size > stack->capacity){
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
        if ( ((shield_t*)((char*)stack->data - sizeof(shield_t)))[0] != shield_value){
            return STACK_ERROR_DATA_LEFT_SHIELD;
        }
        if ( ((shield_t*)((char*)(stack->data + stack->capacity)))[0] != shield_value){
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
            stack->hash = rem_hash;
            stack->data_hash = rem_data_hash;
            return STACK_ERROR_DATA_HASH;
        }
        if (stack->hash != rem_hash){
            stack->hash = rem_hash;
            stack->data_hash = rem_data_hash;
            return STACK_ERROR_HASH;
        }
    #endif // SECOND_LEVEL
    return STACK_ERROR_OK;
}

stack_error_code shield_wall (struct stack_t *stack){
    if (!stack){
        return STACK_ERROR_NO_POINTER;
    }
    ((shield_t*)(char*)stack->data)[0] = shield_value;
    stack->data = (elemen_t*)((char*)stack->data + sizeof(shield_t));
    ((shield_t*)((char*)(stack->data + stack->capacity)))[0] = shield_value;
    return STACK_ERROR_OK;
}

stack_error_code stack_ctor (struct stack_t *stack, int capacity){
    if (stack == NULL){
        return STACK_ERROR_NO_POINTER;
    }
    if (capacity < 0){
        return STACK_ERROR_CAPACITY_LESS_ZERO;
    }
    if (stack->capacity >= 0){
        return STACK_ERROR_CONSTRUCTED;
    }
    #ifdef ZERO_LEVEL

        stack->data = (elemen_t*)calloc(capacity * sizeof(elemen_t), sizeof(elemen_t));

    #else

        stack->data = (elemen_t*)calloc(1, sizeof(shield_t) + capacity * sizeof(elemen_t) + sizeof(shield_t));


    #endif // ZERO_LEVEL


    if (stack->data == nullptr){
        return STACK_ERROR_NO_MEMORY;
    }
    stack->capacity = capacity;
    #ifndef ZERO_LEVEL

        shield_wall(stack);
        stack->left_shield = shield_value;
        stack->right_shield = shield_value;

    #endif // ZERO_LEVEL

    stack->size = 0;

    #ifdef SECOND_LEVEL

        update_hash(stack);

    #endif // SECOND_LEVEL

    return check_stack_here(stack);
}

stack_error_code stack_dtor (struct stack_t *stack){
    if(stack == NULL){
        return STACK_ERROR_NO_POINTER;
    }
    if (stack->data == 0){
        return STACK_ERROR_DESTRUCTED;
    }
    free(stack->data);
    stack->data = nullptr;
    stack->capacity = -1;
    stack->size = -1;
    #ifdef SECOND_LEVEL
        stack->hash = 0;
        stack->data_hash = 0;
    #endif // SECOND_LEVEL

    return STACK_ERROR_OK;
}

stack_error_code stack_resize_more (struct stack_t *stack){
    if (check_stack_here(stack) != STACK_ERROR_OK){
        return check_stack_here(stack);
    }
    int new_capacity = 0;
    if (stack->capacity <= 0){
        new_capacity = 1;
    }
    if (stack->size >= stack->capacity && stack->capacity >= 1){
        new_capacity = optimal_resize_more(stack->capacity);
    }

    if (new_capacity == 0){
        return STACK_ERROR_OK;
    }

    stack->capacity = new_capacity;

    #ifdef ZERO_LEVEL

        stack->data = (elemen_t*) realloc(stack->data, new_capacity * sizeof(elemen_t));

    #else

        stack->data = (elemen_t*)((char*)stack->data - sizeof(shield_t));
        stack->data = (elemen_t*) realloc(stack->data, new_capacity * sizeof(elemen_t) + 2 * sizeof(shield_t));
        shield_wall(stack);

    #endif // ZERO_LEVEL

    #ifdef SECOND_LEVEL

        update_hash(stack);

    #endif // SECOND_LEVEL
    return check_stack_here(stack);
}

int optimal_resize_more (int capacity){
    if (capacity < 10000){
        return capacity * 2;
    }
    if (capacity < 40000){
        return capacity * 1.6;
    }
    if (capacity < 100000){
        return capacity * 1.2;
    }
    return capacity + 5000;
}

stack_error_code stack_resize_less (struct stack_t *stack){
    if (check_stack_here(stack) != STACK_ERROR_OK){
        return check_stack_here(stack);
    }
    int new_capacity = 0;
    if((stack->size) * (reducing_capacity * 2) <= stack->capacity){
        new_capacity = stack->capacity / reducing_capacity;
    }
    if (new_capacity == 0){
        return STACK_ERROR_OK;
    }
    stack->capacity = new_capacity;

    #ifdef ZERO_LEVEL

        stack->data = (elemen_t*) realloc(stack->data, new_capacity * sizeof(elemen_t));

    #else

        stack->data = (elemen_t*)((char*)stack->data - sizeof(shield_t));
        stack->data = (elemen_t*) realloc(stack->data, new_capacity * sizeof(elemen_t) + 2 * sizeof(shield_t));
        shield_wall(stack);

    #endif // ZERO_LEVEL

    #ifdef SECOND_LEVEL
        update_hash(stack);
    #endif // SECOND_LEVEL

    return check_stack_here(stack);
}

stack_error_code stack_push (struct stack_t *stack, elemen_t value){
    if(check_stack_here(stack) != STACK_ERROR_OK){
        return check_stack_here(stack);
    }
    stack_resize_more(stack);
    stack->data[stack->size++] = value;
    #ifdef SECOND_LEVEL
        update_hash(stack);
    #endif // SECOND_LEVEL
    return check_stack_here(stack);
}

elemen_t stack_pop (struct stack_t *stack){
    if (check_stack_here(stack) != STACK_ERROR_OK){
        return stack_poison;
    }
    stack_resize_less(stack);
    stack->size--;
    elemen_t popped = stack->data[stack->size];
    stack->data[stack->size] = stack_poison;
    #ifdef SECOND_LEVEL
        update_hash(stack);
    #endif // SECOND_LEVEL
    return popped;
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

#ifdef SECOND_LEVEL

stack_error_code update_hash(struct stack_t *stack){
    if(!stack){
        return STACK_ERROR_NO_POINTER;
    }

    stack->data_hash = 0;
    stack->data_hash = stack_hash((char*) stack->data, stack->size * sizeof(elemen_t));
    stack->hash = 0;
    stack->hash = stack_hash((char*)stack, 2 * sizeof(shield_t) + 2 * sizeof(hash_t) + sizeof(elemen_t*) + 2 * sizeof(int) );
    //printf("%u %u\n", stack->data_hash, stack->hash);
    return STACK_ERROR_OK;
}

#endif // SECOND_LEVEL

stack_error_code check_stack (struct stack_t *stack, const char* file, const char* function, const int line){
    if (stack_error(stack) != STACK_ERROR_OK){
        stack_dump(stack, file, function, line);
    }
    return stack_error(stack);
}

int stack_dump (struct stack_t *stack, const char* file, const char* function, const int line){
    FILE* stack_error_data = fopen ("stack_error_data.txt", "w");
    if (stack_error_data == NULL)
    {
        return STACK_ERROR_FILE;
    }
    #ifdef ZERO_LEVEL
        fprintf (stack_error_data, "ZERO level of protection\n\n");
    #endif

    #ifdef FIRST_LEVEL
         fprintf (stack_error_data, "FIRST level of protection, with shields\n\n");
    #endif

    #ifdef SECOND_LEVEL
         fprintf (stack_error_data, "SECOND level of protection, with shields and hash\n\n");
    #endif

    fprintf (stack_error_data, "stack <%s> Have %s at %s in (function: %s | line: %d) \n\n", typeid(stack).name(), stack_error_name[stack_error(stack)], file, function, line);

    if (stack->data != 0){
        fprintf (stack_error_data,
                 "Capacity = %d\n"
                 "Size = %d\n"
                 "Pointer of data <%s> = %p\n\n",
                  stack->capacity, stack->size, typeid(*(stack->data)).name(), stack->data);
        #ifdef SECOND_LEVEL

                fprintf (stack_error_data, "hash = %llu, data_hash = %llu\n", stack->hash, stack->data_hash);
        #endif

        #ifndef ZERO_LEVEL

            fprintf (stack_error_data,
                    "left_shield       = %llx\n"
                    "right_shield      = %llx\n"
                    "left_data_shield  = %llx\n"
                    "right_data_shield = %llx\n\n",
                    stack->left_shield, stack->right_shield,
                    ((shield_t*)((char*)stack->data - sizeof(shield_t)))[0],
                    ((shield_t*)((char*)stack->data + stack->capacity * sizeof(elemen_t)))[0]);
        #endif // ZERO_LEVEL
    }
    fprintf(stack_error_data, "data values:\n");
    for (int i = 0; i < stack->size; i++){
        fprintf (stack_error_data, "  data[%d] = %f \n", i, stack->data[i]);
    }

    fprintf(stack_error_data, "That's it...\n");

    fclose (stack_error_data);

    return STACK_ERROR_OK;
}

#endif
