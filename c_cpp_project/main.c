#include <locale.h>
#include <stdio.h>
#include <string.h>

#include "test.h"

#include <stdint.h>
#include <stddef.h>

#define ARR_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
/*   
  setlocale(LC_ALL, "zh_CN.UTF-8"); 
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdint.h>

/*
 * 因为要是有个宏批量生成，不能用do while，要用inline代替不展开的"宏+do"
 * 用## 连接符，把参数展开成字符串来生成函数名
 * 直接在h文件里面写函数的定义，inline确保了高效（达到define的效果）
 * 都是对一个数据的操作！！
*/
#define __VALUE_GET_TEMPLATE(_type) \
static inline _type get_##_type(uint8_t *data) \
{ \
    return *(_type *)data; \
}

#define __VALUE_PUT_TEMPLATE(_type) \
static inline void put_##_type(uint8_t *data, _type value) \
{ \
    *(_type *)data = value; \
}

// 完成对数据的操作，然后自增地址
#define __VALUE_GET_INC_TEMPLATE(_type) \
static inline _type get_##_type##_inc(uint8_t **data) \
{ \
    uint8_t *temp = *data; \
    *data = temp + sizeof(_type); \
    return get_##_type(temp); \
}

#define __VALUE_PUT_INC_TEMPLATE(_type) \
static inline void put_##_type##_inc(uint8_t **data, _type value) \
{ \
    uint8_t *temp = *data; \
    *data = temp + sizeof(_type); \
    put_##_type(temp, value); \
}

// 把函数框架当成宏，快速生成
#define __VALUE_GENERATE_FUNC(_frame) \
    _frame(uint8_t) \
    _frame(uint16_t) \
    _frame(uint32_t)

__VALUE_GENERATE_FUNC(__VALUE_GET_TEMPLATE)
__VALUE_GENERATE_FUNC(__VALUE_PUT_TEMPLATE)
__VALUE_GENERATE_FUNC(__VALUE_GET_INC_TEMPLATE)
__VALUE_GENERATE_FUNC(__VALUE_PUT_INC_TEMPLATE)

#endif

/*
| magic | channel | length | data     | crc16 |
| 1     | 1       | 2      | {length} | 2     |
| 0xaa  | 0~255   | 0~65535|          |       |
*/

#define COM_PROTO_MAGIC             0xaa
#define COM_PROTO_MAGIC_OFFSET      0
#define COM_PROTO_MAGIC_SIZE        1
#define COM_PROTO_CHANNEL_OFFSET    1
#define COM_PROTO_CHANNEL_SIZE      1
#define COM_PROTO_LENGTH_OFFSET     2
#define COM_PROTO_LENGTH_SIZE       2
#define COM_PROTO_CRC_OFFSET        4
#define COM_PROTO_CRC_SIZE          2
#define COM_PROTO_DATA_OFFSET       4
#define COM_PROTO_BASE_SIZE         (COM_PROTO_MAGIC_SIZE + COM_PROTO_CHANNEL_SIZE + COM_PROTO_LENGTH_SIZE + COM_PROTO_CRC_SIZE)
#define COM_PROTO_HEAD_SIZE         (COM_PROTO_MAGIC_SIZE + COM_PROTO_CHANNEL_SIZE + COM_PROTO_LENGTH_SIZE)

uint16_t crc16(const char* data, uint16_t length) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= (uint8_t)data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}





/**
 * @brief 动态内存分配创建协议包
 * 
 * 填充包头，通道，长度
 * @param channel 
 * @param length 
 * @return 包头地址
 */
uint8_t *com_proto_create(uint8_t channel, uint16_t length)
{
    uint8_t *proto = malloc(length + COM_PROTO_BASE_SIZE);
    if (proto == NULL)
    {
        return NULL;
    }

    put_uint8_t(&proto[COM_PROTO_MAGIC_OFFSET], COM_PROTO_MAGIC);
    put_uint8_t(&proto[COM_PROTO_CHANNEL_OFFSET], channel);
    put_uint16_t(&proto[COM_PROTO_LENGTH_OFFSET], length);

    return (proto);
}

/**
 * @brief 释放动态内存
 * @param proto 
 */
void com_proto_free(uint8_t *proto)
{
    free(proto);
}

/**
 * @brief 完整包填充CRC校验码
 * @param proto 
 * @return 
 */
uint8_t *com_proto_fill_crc(uint8_t *proto)
{
    uint8_t *pproto = proto;
    uint16_t length = get_uint16_t(&pproto[COM_PROTO_LENGTH_OFFSET]);

    uint16_t crc = crc16(&pproto[COM_PROTO_CHANNEL_OFFSET], length + COM_PROTO_CHANNEL_SIZE + COM_PROTO_LENGTH_SIZE);

    put_uint16_t(&pproto[length + COM_PROTO_CRC_OFFSET], crc);

    return (pproto);
}

/**
 * @brief 校验包crc
 * @param proto 
 * @param length 
 * @return 布尔值
 */
bool com_proto_verify(uint8_t *proto, uint16_t length)
{
    if (proto == NULL || length < COM_PROTO_BASE_SIZE)
    {
        return false;
    }

    if (proto[COM_PROTO_MAGIC_OFFSET] != COM_PROTO_MAGIC ||
        get_uint16_t(&proto[COM_PROTO_LENGTH_OFFSET]) != length - COM_PROTO_BASE_SIZE)
    {
        return false;
    }

    uint16_t crc = crc16((char *)&proto[COM_PROTO_CHANNEL_OFFSET], length - COM_PROTO_MAGIC_SIZE - COM_PROTO_CRC_SIZE);
    uint16_t crc2 = get_uint16_t(&proto[length - COM_PROTO_CRC_SIZE]);
    return crc == crc2;
}

uint16_t com_proto_get_length(uint8_t *proto)
{
    return get_uint16_t(&proto[COM_PROTO_LENGTH_OFFSET]) + COM_PROTO_BASE_SIZE;
}


// 模拟ESP相关常量
#define CHANNEL_ESP 1
#define ESP_UUID_LEN_16 2
#define SVC_CTRL_CHAR_MISC_UUID 0x1234
#define LOG_TAG "TEST"

// 模拟ESP日志函数
#define ESP_LOG_BUFFER_HEX(tag, buffer, len) print_hex_buffer(tag, buffer, len)
#define ESP_LOGW(tag, format, ...) printf("[%s] WARNING: " format "\n", tag, ##__VA_ARGS__)

// CRC16简单实现（用于测试）

// 打印十六进制缓冲区的函数
void print_hex_buffer(const char* tag, const uint8_t* buffer, uint16_t length) {
    printf("[%s] 数据长度: %d 字节\n", tag, length);
    printf("[%s] 十六进制: ", tag);
    for (int i = 0; i < length; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
    
    // 可视化ASCII表示
    printf("[%s] ASCII视图: ", tag);
    for (int i = 0; i < length; i++) {
        if (buffer[i] >= 32 && buffer[i] <= 126) {
            printf("%c", buffer[i]);
        } else {
            printf(".");
        }
    }
    printf("\n\n");
}

// 模拟UART发送函数
void com_uart_send(uint8_t* data, uint16_t length) {
    printf("=== UART发送数据 ===\n");
    print_hex_buffer("UART-TX", data, length);
    
    // 分析协议包结构
    printf("协议包结构分析:\n");
    printf("Magic: 0x%02X\n", data[COM_PROTO_MAGIC_OFFSET]);
    printf("Channel: %d\n", data[COM_PROTO_CHANNEL_OFFSET]);
    printf("Length: %d\n", get_uint16_t(&data[COM_PROTO_LENGTH_OFFSET]));
    printf("Data offset: %d\n", COM_PROTO_DATA_OFFSET);
    
    // 显示数据部分
    uint16_t data_len = get_uint16_t(&data[COM_PROTO_LENGTH_OFFSET]);
    if (data_len >= ESP_UUID_LEN_16) {
        uint16_t uuid = get_uint16_t(&data[COM_PROTO_DATA_OFFSET]);
        printf("UUID: 0x%04X\n", uuid);
        printf("原始数据: ");
        for (int i = COM_PROTO_DATA_OFFSET + ESP_UUID_LEN_16; 
             i < COM_PROTO_DATA_OFFSET + data_len; i++) {
            printf("%02X ", data[i]);
        }
        printf("\n");
    }
    printf("==================\n\n");
}

// 应用层回调函数实现
static void svc_ctrl_name_callback(uint8_t *data, uint16_t length)
{
    printf("=== svc_ctrl_name_callback 调用 ===\n");
    ESP_LOG_BUFFER_HEX("name-rx", data, length);

    uint8_t *proto = com_proto_create(CHANNEL_ESP, length + ESP_UUID_LEN_16);
    if (!proto) 
    {
        ESP_LOGW(LOG_TAG, "svc_name com_proto_create failed");
        return;
    }

    printf("创建的协议包初始状态:\n");
    print_hex_buffer("PROTO-INIT", proto, COM_PROTO_BASE_SIZE + length + ESP_UUID_LEN_16);

    uint8_t *data_ptr = proto + COM_PROTO_DATA_OFFSET;
    put_uint16_t_inc(&data_ptr, SVC_CTRL_CHAR_MISC_UUID);
    memcpy(data_ptr, data, length);
    
    printf("填充UUID和数据后:\n");
    print_hex_buffer("PROTO-FILLED", proto, COM_PROTO_BASE_SIZE + length + ESP_UUID_LEN_16);

    com_proto_fill_crc(proto);
    
    printf("填充CRC后的完整协议包:\n");
    uint16_t total_length = com_proto_get_length(proto);
    print_hex_buffer("PROTO-CRC", proto, total_length);

    // 验证CRC
    bool is_valid = com_proto_verify(proto, total_length);
    printf("CRC校验结果: %s\n", is_valid ? "通过" : "失败");

    com_uart_send(proto, total_length);
    com_proto_free(proto);
    
    printf("=== svc_ctrl_name_callback 结束 ===\n\n");
}

// 测试函数
void run_tests() {
    printf("=====================================\n");
    printf("协议包测试程序\n");
    printf("=====================================\n\n");

    // 测试1: 简单文本数据
    printf("测试1: 简单文本数据\n");
    uint8_t test_data1[] = "Hello ESP32";
    svc_ctrl_name_callback(test_data1, strlen((char*)test_data1));

    // 测试2: JSON格式数据
    printf("测试2: JSON格式数据\n");
    uint8_t test_data2[] = "{\"cmd\":\"get_status\",\"id\":123}";
    svc_ctrl_name_callback(test_data2, strlen((char*)test_data2));

    // 测试3: 二进制数据
    printf("测试3: 二进制数据\n");
    uint8_t test_data3[] = {0x01, 0x02, 0x03, 0x04, 0xFF, 0xFE};
    svc_ctrl_name_callback(test_data3, sizeof(test_data3));

    // 测试4: 空数据
    printf("测试4: 空数据\n");
    uint8_t test_data4[] = "";
    svc_ctrl_name_callback(test_data4, 0);

    printf("=====================================\n");
    printf("所有测试完成\n");
    printf("=====================================\n");
}

int main() {  setlocale(LC_ALL, "zh_CN.UTF-8"); 

    run_tests();
    return 0;
}