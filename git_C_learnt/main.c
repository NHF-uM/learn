#include <locale.h>
#include <stdio.h>
#include <string.h>

#include "test.h"

#include <stdint.h>
#include <stddef.h>

#define ARR_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

int main()
{
    setlocale(LC_ALL, "zh_CN.UTF-8");

    int a = 0, b = 0;
    a = 10;
    b = 20;
    printf("a + b = %d\n", a + b);

    
    return 0;
}