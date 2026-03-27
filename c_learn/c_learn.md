# C

## 数据类型

### 作用

- 决定开辟的内存大小
  - `float`: 4字节
  - `double`: 8字节

- 决定数据在内存中如何存储（存进去）
  - 整型
    - 存储形式（有无符号位看数据类型uint or int）
      ![整数的存储形式](integer_storage_in_memory.png)
      
      - 原码
      - 反码（前提是有符号的负数）：符号位不变，其余位按位取反
      - 补码（前提是有符号的负数）：第一位是符号位，0为正数，**所有内存中的整数都是以补码形式存储的，所有运算也是用的补码（加减乘除，&，^，|，~【符号位也反转】）**
      定义`1000 0000`为-128的补码（按照补码规则算出来是-0）
    - “放”与“取”
      - 放：根据空间大小，把补码截断，放入内存
      - 取：根据类型判断，是否要转化为原码
    - 码位溢出（计算过程，不包括原补码转换过程）：
      - 正数和正数相加，如果计算结果的符号位为1，说明发生了码位溢出，需要给一位，符号位若为0则舍去溢出位
      - 负数和负数相加，如果计算结果的符号位为0，说明发生了码位溢出，需要给一位，符号位若为1则舍去溢出位
    
      ```c
      unsigned char a = 255; // 1111 1111
      unsigned char b = 1;   // 0000 0001
      unsigned char c = a + b;
      结果给1位：1 0000 0000；但是类型只有8bit空间，所以结果是0000 0000
      ```
    
      ```c
      unsigned char a = 300; // 1 0010 1100
      只取8位：0010 1100；结果是44
      ```
    
      ```c
      signed char a = 127; // 0111 1111
      a = a + 1; // 1000 0000
      结果是-128
      ```
    
    - 类型提升：
      - 场景：
        - 表达式中：函数调用（`printf`），赋值（**排除声明:`char a = 128`**），运算，条件判断
        - 函数参数传递
    
      - 规则：根据寄存器的位数对**补码**进行提升
        - 有符号 
          - 符号位为0，高位补0
          - 符号位为1，高位补1 
        - 无符号
          - 高位补0
    
      - 示例：
        ```c
          #include <stdio.h>
          int main()
          {
              char a = -128;
              printf("%u\n", a);
              return 0;
          }
          --> -128的补码存储（1000 0000）
          -->printf时类型提升（1111 1111 1111 1111 1111 1111 1000 0000）
          -->用%u来解析（原码）4294967168
        ```
    
        ```c
          int main()
          unsigned char i = 0;
          for(i = 0; i <= 255; i++)
          {
            printf("%u\n", i);
            Sleep(50); // 延时50ms
          }
          return 0;
        
          i一直加到255
          -->255 1的补码进行提升并且相加
          -->1 0000 0000 0000 0000 0000 0000 0000 0000
          -->结果进行高位截断
          -->0000 0000提升并且给printf
        ```
    
        ```c
        char a = 127,b = 2;
        char c = a+b;
        printf("%d\n", c);
        有符号正数：原=补
        ——>提升并且相加之后是（还有0，补到32位）0 1000 0001
        ——>截断放到c的内存，1000 0001(**如果不定义c，直接打印a+b，则不需要截断，结果为129！！！！！！**)
        ——>取出来，进行类型提升，1111 1111 1111 1111 1111 1111 1000 0001（补码）用%d解析，打印结果为-127
        ```
  
- 决定如何解析一段地址中的连续数据（取出来）
  - 根据类型判断最高位是否为符号位
  - 示例：
    ```c
    // 大小端转换，解析数据
    u8 buf[BUF_SIZE];
    u32 data = *(u32 *)buf;
    ```

## 字符串

### `<string.h>` 相关

- `strlen` 返回 `size_t`，是无符号的
  - 无符号数相加减结果仍为无符号数
  - 示例：`(size_t)4 - (size_t)6 > 0`

### 字符串常量和字符数组

- 字符串常量：
  `char *str = "hello world"`   因为是常量，所以无法修改内容;
- 字符数组：
  `char str[10] = "hello world"`

## 宏定义

### 用法

- 替换文本（仅文本，非字符串）
  
- 链接符
  
  ```c
  #define CAT(a, b) a ## b
  
  int main()
  { 
    int state = 0;
    printf("%d", CAT(sta, te));  
  }
  ```

- 可变参数宏

  ```c
  #define TRY_CALL(f, ...) \
  if (f != NULL)           \
  f(__VA_ARGS__)
  ```

  1. ...在宏定义的参数列表末尾时，是 C99可变参数宏的专用语法，表示：宏接受「0 个或多个任意类型的后续参数」（参数个数≥0，类型无限制，如 int、char*、bool、结构体指针等）
  2. __VA_ARGS__是 C99 标准为可变参数宏预留的预处理器内置宏（特殊标识符）, 在宏体中，原封不动地替换宏调用时...对应的实际参数列表
  3. 宏的可变参数和函数的可变参数（stdarg.h）不相等！！

### 缺点

- 宏定义占用代码空间大，每一次都是完整的替代
- 调试困难
  - 调试时看到的代码和源代码有区别，不会显示INTERVAL，只会显示100
  - 无法打断点，如果是多行宏定义，看不到过程
- 运算符优先级问题

## 枚举

- 默认从0开始递增，也可以手动指定初始值，后续值递增1

- 分为 **“有名枚举”** 和 **“匿名枚举”**

  ```c
  // 有名枚举，创建变量使用
  enum Color
  {
      RED,
      GREEN,
      BLUE
  };
  
  Color c = RED;
  
  // 匿名枚举
  enum
  {
      MONDAY,
      TUESDAY,
      WEDNESDAY
  };
  ```

## 宏定义和枚举

- 宏定义是简单的文本替换，枚举是类型安全的常量集合

 - 有逻辑的相关常量，使用枚举更合适

 - 需要文本替换、条件编译、非整型常量、代码片段复用 → 只能用宏定义，否则**优先使用枚举**

### 宏展开

```c
#define _STR(R) #R
#define STR(R)  _STR(R)

// 定义一个宏常量
#define BAUD_RATE 115200
// 定义一个普通变量
int baud = 9600;

// 直接用底层宏：不展开宏，仅字面量字符串化
printf("底层宏：%s\n", _STR(BAUD_RATE)); // 输出：底层宏：BAUD_RATE
printf("底层宏：%s\n", _STR(baud));     // 输出：底层宏：baud

// 用上层宏：先展开宏，再字符串化
printf("上层宏：%s\n", STR(BAUD_RATE));  // 输出：上层宏：115200
printf("上层宏：%s\n", STR(baud));      // 输出：上层宏：baud（普通变量无宏可展开，和底层宏一致）
```

### 集大成者（宏拼接和宏的妙用）

```c
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

```

## 结构体

声明结构体的时候只是在定义一个数据类型，不涉及内存的分配

### 字节对齐

- 小对齐

  每个成员都需要判断是否对齐（编译器决定是几字节对齐！！），是否需要在后面补字节空间
  ```c
  struct    // 假设这里编译器是4字节对齐
  {
    u8 a;   // 1字节
            // 填充3字节，4字节对齐
    int b;  
            // 4字节
    u8 c;
            // 填充3字节，4字节对齐
            // 共12字节，符合%4（int） = 0
  } test;
  ```
  ```c
  typedef struct
  {
    struct
    {
      struct ALLOC_HDR *ptr;  // 地址4字节
      unsigned int size;      // 4
    } s;  // 刚好8

    unsigned int align;       // 4
    unsigned int pad;         // 4
  } ALLOC_HDR;  // 刚好16 % 4 = 0
  ```
tips：定义结构体的时候，把相同大小的成员放在一起，可以减少填充字节数

- 大对齐
  
  总字节大小必须是其最宽成员的字节数的整数倍

  ⚠️：结构体的对齐数 = 最大成员的对齐数，所以ALLOC_HDR

## 联合体

共用一片内存空间，大小为最大成员的大小！！

<details>
<summary>ESP-IDF 中，GATT协议传递的数据多种多样，不是每个数据包都包含所有字段</summary>

```c
typedef union {
    /**
     * @brief ESP_GATTS_REG_EVT
     */
    struct gatts_reg_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t app_id;                /*!< Application id which input in register API */
    } reg;                              /*!< Gatt server callback param of ESP_GATTS_REG_EVT */

    /**
     * @brief ESP_GATTS_READ_EVT
     */
    struct gatts_read_evt_param {
        uint16_t conn_id;               /*!< Connection id */
        uint32_t trans_id;              /*!< Transfer id */
        esp_bd_addr_t bda;              /*!< The bluetooth device address which been read */
        uint16_t handle;                /*!< The attribute handle */
        uint16_t offset;                /*!< Offset of the value, if the value is too long */
        bool is_long;                   /*!< The value is too long or not */
        bool need_rsp;                  /*!< The read operation need to do response */
    } read;                             /*!< Gatt server callback param of ESP_GATTS_READ_EVT */


    /**
     * @brief ESP_GATTS_WRITE_EVT
     */
    struct gatts_write_evt_param {
        uint16_t conn_id;               /*!< Connection id */
        uint32_t trans_id;              /*!< Transfer id */
        esp_bd_addr_t bda;              /*!< The bluetooth device address which been written */
        uint16_t handle;                /*!< The attribute handle */
        uint16_t offset;                /*!< Offset of the value, if the value is too long */
        bool need_rsp;                  /*!< The write operation need to do response */
        bool is_prep;                   /*!< This write operation is prepare write */
        uint16_t len;                   /*!< The write attribute value length */
        uint8_t *value;                 /*!< The write attribute value */
    } write;                            /*!< Gatt server callback param of ESP_GATTS_WRITE_EVT */

    /**
     * @brief ESP_GATTS_RESPONSE_EVT
     */
    struct gatts_rsp_evt_param {
        esp_gatt_status_t status;       /*!< Operation status */
        uint16_t handle;                /*!< Attribute handle which send response */
    } rsp;                              /*!< Gatt server callback param of ESP_GATTS_RESPONSE_EVT */

    /**
     * @brief ESP_GATTS_CREAT_ATTR_TAB_EVT
     */
    struct gatts_add_attr_tab_evt_param{
        esp_gatt_status_t status;       /*!< Operation status */
        esp_bt_uuid_t svc_uuid;         /*!< Service uuid type */
        uint8_t svc_inst_id;            /*!< Service id */
        uint16_t num_handle;            /*!< The number of the attribute handle to be added to the gatts database */
        uint16_t *handles;              /*!< The number to the handles */
    } add_attr_tab;                     /*!< Gatt server callback param of ESP_GATTS_CREAT_ATTR_TAB_EVT */

    …………………………
    …………………………
} esp_ble_gatts_cb_param_t;
```
## 关键字

### const

不是真正的常量，`const u8 size = 100;`  `uint8_t arr[size]`会报错

const的修饰对象：


- 修饰变量：
  变量不能**直接修改**(所以可以地址修改！)

- 修饰指针
  不修饰数据类型！所以右边离谁近修饰谁
  `const uint8_t *p = arr;`
  `uint8_t const *p = arr;`
  `uint8_t *const p = arr;`

### volatile

一个变量可能被外部（硬件）修改，尽管我们写的语句可能不涉及这个变量的内容修改。这时需要cpu重新从**内存**（真实存储位置，而不是本地备份位置）读取这个变量的最新值

### sizeof

获取变量的数据类型的大小。

避免数组进行指针退化的其中一种方式`sizeof(arr) = 整个数组所占的字节数`

## 计算机框架

### 运算单元

- CPU：
  从内存中取数据，运算，将结果存回内存（直接从硬件读取速度慢）

### 存储单元

#### 内存

- 特点：
  速度快（电子运动），掉电丢失

##### FLASH

- RO（Read Only）段
  - text段：
    代码段+只读常量

##### ROM

##### RAM（临时随机存储器Random Access Memory）

- RO代码区
  - text段
  - rodata段
  
- 已初始化区
  - data段：已初始化的全局变量和静态变量
  
- 未初始化区
  - bss段：未初始化的全局变量和静态变量
  
- 栈

- 堆

编译分区：
- .text：代码段（RO 段）
- .rodata：只读数据（RO 段）
- .data：已初始化的全局/静态变量（RW 段）
- .bss：未初始化或初始化为0的全局/静态变量（ZI 段，但是也是rw的）
  
#### 硬盘

- 特点：
  存储空间大，速度慢（机械运动）

## 字节对齐

字节对齐指的是被访问的地址（或者起始地址）被N字节整除
它是为内存访问服务的————它确保 CPU 能高效（非字节对齐访问 可能需要读多几次）、安全地读写数据。没有合理的对齐，内存访问可能变慢、出错，甚至导致程序崩溃。

32位单片机普遍使用 4 字节对齐，有的必须要字节对齐内存访问，有的支持非对齐内存访问（cotex-m3、m4）

tips：任意能被4整除的数，其二进制的最低两位一定是 0

## 内存分配

### 静态分配

### 动态分配

需要手动管理，容易造成内存碎片化，访问速度慢？？？

- 分配在堆区（手动分配的特点，所以可以动态调整，减少对程序的影响）
- `malloc` 函数使用必须要检查返回值（是否申请成功）
- `free` 函数需要及时使用（否则可能导致内存泄露）；指针也要null放空（否则会导致野指针）
  - 内存泄露：
    大量内存申请之后未得到及时释放，导致内存空间不足
- `realloc` 重新分配内存大小
  - 原空间后续空间足够：
    在原空间后增加
  - 原空间后续空间不够：
    创建新的空间，将原空间内容复制过去，释放原空间（返回值可能是null，所以不能用原空间地址）

#### 柔性数组

结构体的最后一个成员是未知大小的数组

优势：
  - 可以动态调整数组大小，减少内存浪费（不再需要烦恼缓冲区大小，特别是当长度变化大的时候）；否则就要结构体里面嵌套指针，需要多次释放避免内存泄露！

特点：
 - 该结构体至少要有一个成员，才能到柔性数组
 - sizeof的返回值不包括柔性数组，需要手动加上柔性数组的大小（如果需要的话）
 - 柔性数组要和动态内存分配结合使用，不能直接定义实例，如`ble_notify_t notify`

## ARM汇编

### 语法
 - `MOV R0, #0`：将立即数0加载到寄存器R0中
 - `LDR R1, =0x20000000`：将地址0x20000000加载到寄存器R1中
 - `STR R0, [R1, #4]`：将寄存器R0的值存储到寄存器“R1+4字节”指向的内存地址（方括号表示内存地址）中
 - 

## 其他小知识

### 数组的指针退化

只有sizeof和&会保持数组的整体性，其余都会退化成首元素的指针
```c
#define ARR_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
void func(int *data)
{
    printf(("length: %d\n"), ARR_SIZE(data));
}

// 结果是2：指针大小（x86）8 / 4 = 2
```
`int arr[3][3]`:
`&data + 1`步长是整个数组的大小 <—— &data 是指向整个数组的指针，类型是 int (*)[3][3]
`data + 1`步长是首元素的类型，即int (*)[3];

### 回调函数（钩子函数）

`int (*spp_app_main(int x))(void);` 声明一个名为 spp_app_main 的函数，该函数接受一个 int 参数，返回一个指向无参数且返回 int 的函数指针。
`int *(*spp_app_main(int x))(void);` 返回的是指向无参数且返回 int*（整数指针）的函数指针。
`typedef int (*FuncType)(void);` `int (*spp_app_main(int x))(void);` 函数返回一个指向无参数且返回 int 的函数指针。

### 运算优先级

按照参与**运算的元素的个数**来划分：
一元运算符 > 二元运算符 > 三元运算符
⚠️：
 - 括号优先级最高
 - ++ 和 -- 参与运算的元素为一个，属于一元运算符

### 入参参数拷贝

⚠️：这里虽然是指针，但是本质还是值传递

```c
static inline void put_uint16_inc(uint8_t *buf, uint16_t val)
{
    buf = (uint16_t *)buf;
    *buf = val;
    buf += sizeof(uint16_t);
}

/* 上面的函数实际上并没有修改到buf，buf虽然是地址，但也是从实参拷贝为形参，要改为： */

static inline void put_uint16_inc(uint8_t **buf, uint16_t val)
{
    uint8_t *p = (uint16_t *)(*buf);
    *p = val;
    *buf += sizeof(uint16_t);
}
```

### 头文件引用

采用“编译时路径”，即根据构建文件（makefile/cmake/c_cpp.json）的include_path为**根目录**（不包括它本身）来查找（include_path也是按顺序查找）

### 速记

- 1kb = 1024bytes = 0x400 = 4 * 256（0x100）
- 4kb = 4096bytes = 0x1000