#include <locale.h>
#include <stdio.h>
#include <string.h>

#include "test.h"

#include <stdint.h>
#include <stddef.h>

// 通用字节反转函数
static void ReverseEndian(void *buf, size_t len) {
    uint8_t *pArr = (uint8_t *)buf;
    for (size_t i = 0, j = len - 1; i < j; i++, j--) {
        pArr[i] ^= pArr[j];
        pArr[j] ^= pArr[i];
        pArr[i] ^= pArr[j];
    }
}

// 通用大小端转换宏
#define REVERSE_ENDIAN(a) ( { \
    typeof(a) _a = (a); \
    ReverseEndian((void*)&_a, sizeof(_a)); \
    _a; \
})

uint32_t func(void *ptr, uint8_t len)
{
    uint8_t *bytes = (uint8_t *)ptr;    // void* 不能对数据进行操作（没有指定数据类型，无法解析）
    for (uint16_t i = 0, j = len - 1; i < j; i++, j--)
    {
        bytes[i] ^= bytes[j];
        bytes[j] ^= bytes[i];
        bytes[i] ^= bytes[j];
    }
}

char *str = "hello world";

int main()
{
    setlocale(LC_ALL, "zh_CN.UTF-8");

    // uint8_t buf[4] = {0x01, 0x00, 0x00, 0x00};
    // uint32_t value = *(uint32_t *)buf;
    // printf("%d", value);
    
    // uint16_t value16 = 0x1234;
    // // printf("16位值: 0x%04x -> 转换后: 0x%04x\n", 
    // //        value16, (uint16_t)REVERSE_ENDIAN(value16));
    
    // func(&value16, sizeof(value16));
    // printf("16位值: 0x%04x -> 逆序后: 0x%04x\n", 
    //        value16, 0x12);
    int *p = NULL;
    printf("%d", *p);

    return 0;
}

