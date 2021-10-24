#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack_functions.h"
#if defined (ZERO_LEVEL) || defined (FIRST_LEVEL) || defined (SECOND_LEVEL)

int main()
{
    struct stack_t stack;
    printf("%s\n", stack_error_name[stack_ctor(&stack, 1000000000)]);
    printf("%s\n", stack_error_name[stack_ctor(&stack, -666)]);
    printf("%s\n", stack_error_name[stack_ctor(&stack, 0)]);
    printf("%s\n", stack_error_name[stack_ctor(&stack, 0)]);
    for (int i = 0; i < 1000; i++){
        if (i == 28){
            //stack.data[0] = 100;
            //stack.data[-1] = 0;
            //stack.data[32] = 0;
            //*(&stack.data - 1) = 0;
            //*(&stack.hash - 1) = 7;
            //*(((char*)&stack.size) + 8) = 206;
            *(((char*)&stack.size) + 13) = 206; // actually not error
            printf("%d %x %llx\n", *(int*)((char*)&stack.size + 4), *(int*)((char*)&stack.size + 4), *(shield_t*)((char*)&stack.size + 4));
            printf("%p %p %p %p %p %p %p\n", &stack.left_shield, &stack.hash, &stack.data_hash, &stack.data, &stack.capacity, &stack.size, &stack.right_shield);
        }
        if(stack_push(&stack, i) != STACK_ERROR_OK){
            printf("Ooops, it seems to me that somewhere is an error...\n");
            return 0;
        }
    }
    printf("!Pushed\n");
    while(stack.size){
        printf("was: %.0f size = %d\n", stack_pop(&stack), stack.size);
    }
    printf("%s\n", stack_error_name[stack_dtor(&stack)]);
    printf("%s\n", stack_error_name[stack_dtor(&stack)]);
    return 0;
}

#endif
