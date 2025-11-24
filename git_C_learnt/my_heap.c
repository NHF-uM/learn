 #include <stdint.h>
#include <stddef.h>

// 使用首次适配算法（First Fit）：
// 从空闲链表的当前位置开始，顺序查找“第一个”足够大的空闲块进行分配。

typedef struct ALLOC_STRUCT
{
    struct header_block // 嵌套结构体，便于管理和封装
    {
        ALLOC_STRUCT *next;
        uint32_t size;  // 单位是信息块的长度，方便进行管理！！！！！！(包括了)
    }header_block;

    // 字节填充，保证占用16字节
    uint32_t align;
    uint32_t pad;
} ALLOC_STRUCT;

#define ALLOC_BUFF_SIZE 4096
#define S_NEXT_BLOCK(p)   p->header_block.next
#define S_SIZE(p)         p->header_block.size

__attribute__((aligned (4))) // 保证内存池四字节对齐：起始地址 % 4 = 0（GCC编译器）
uint8_t alloc_buff[ALLOC_BUFF_SIZE];

ALLOC_STRUCT *base;     // 第一个头部块
ALLOC_STRUCT *freep;    // 管理空闲块的某个动态改变的链表节点（循环链表的时候，freep随便改变）

void alloc_init(void)
{
    base = (ALLOC_STRUCT *)alloc_buff; // 把信息头放进缓冲区
    base->header_block.next = base;    // 形成循环链表，简化链表的边界条件处理 
    base->header_block.size = (ALLOC_BUFF_SIZE) / (sizeof(ALLOC_STRUCT));

    freep = base;
}

void *malloc(uint32_t nbytes)
{
    if (freep == NULL)
        alloc_init();

    ALLOC_STRUCT *p = S_NEXT_BLOCK(freep);    // 不修改原指针
    ALLOC_STRUCT *prev = freep;               // 不修改原指针，取出——修改——放回去
    uint32_t size_need = (nbytes + (sizeof(ALLOC_STRUCT) - 1)) / sizeof(ALLOC_STRUCT) + 1;  // 向上取整，并且加上一个结构体

    for (;;prev = p, p = S_NEXT_BLOCK(p))
    {
        if (S_SIZE(p) >= size_need) // 刚好够大，把p移除出链表
        {
            if (S_SIZE(p) == size_need)
            {
                S_NEXT_BLOCK(prev) = S_NEXT_BLOCK(p);
            }
            else    // 切割内存，把分配出去的内存信息块移除出链表
            {
                S_SIZE(p) -= size_need; // 这里的p那个节点的结构体的地址，这里就完成了对剩余空间的size的调整！！！！！
                p += S_SIZE(p);
                S_SIZE(p) = size_need;

                // 不修改原指针，所以剩余空间的节点仍然在链表里面的！！！！！
                freep = prev;   // 脑子模拟的是一次就找到了，但是实际上如果要循环几次才找到p的话，prev早就不等于freep了（保持prev是修改过的前一个节点）
            }

            return (void *)(p + 1);
        }

        if (p == freep) // 遍历完全链表，没有合适的内存块，退出循环
        {
            return NULL;
        }
    }
}

void free(void *str)
{
    ALLOC_STRUCT *p_user = (ALLOC_STRUCT *)str - 1;
    ALLOC_STRUCT *p = freep;
    
    // 当p > p_usrr > p.next 或者 循环链表的边界讨论(两种绕回到开头的情况) 退出循环，这里简化得太好了
	for (; !((p_user > p) && (p_user < S_NEXT_BLOCK(p))); p = S_NEXT_BLOCK(p))
	{
		if ((p >= S_NEXT_BLOCK(p)) && ((p_user > p) || (p_user < S_NEXT_BLOCK(p))))
		{
			/*
				当一个块，
				p>=p->s.ptr 本身起始地址指针大于下一块地址指针
				bp>p 要释放的块，地址大于P
				bp<p->s.ptr 要释放的块，地址小于下一块
			*/
			break; /* freed block at start or end of arena */
		}
	}

    if (p_user + S_SIZE(p_user) == S_NEXT_BLOCK(p))
    {
        S_SIZE(p_user) += S_SIZE(S_NEXT_BLOCK(p));
        S_NEXT_BLOCK(p_user) = (p->header_block.next)->header_block.next;
    }
    else 
    {
        S_NEXT_BLOCK(p_user) = S_NEXT_BLOCK(p);
    }

    if (p + S_SIZE(p) == p_user)
    {
        S_SIZE(p) += S_SIZE(p_user);
        S_NEXT_BLOCK(p) = S_NEXT_BLOCK(p_user);
    }
    else
    {
        S_NEXT_BLOCK(p) = p_user;
    }

    freep = p;
}