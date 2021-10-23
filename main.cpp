#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack_functions.h"
#if defined (ZERO_LEVEL) || defined (FIRST_LEVEL) || defined (SECOND_LEVEL)

int main()
{
    struct stack_t stack;
    stack_ctor(&stack, 0);
    for (int i = 0; i < 1000; i++){
        stack_push(&stack, i);
    }
    printf("!Pushed\n");
    stack_dump(&stack, __FILE__, __func__, __LINE__);
    while(stack.size){
        printf("was: %.0f size = %d\n", stack_pop(&stack), stack.size);
    }
    stack_dtor(&stack);
    printf("!End\n");
    return 0;
}

#endif
