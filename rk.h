/*
 * Peripheral Port Manager (Pangolin) for GDK8 and YourLand Code Book 
 *
 * Copyright (C) 2021-2024 GEGU Tech. Ltd.
*/
#ifndef __PANGOLIN_RK_COMMON_H__

#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#define GRF_HIWORD_UPDATE(val, mask, shift) \
        ((val) << (shift) | (mask) << ((shift) + 16))

static void __raw_writel(uint32_t b, volatile void* addr)
{
    *(volatile uint32_t*)addr = b;
}

static void writel(uint32_t b, volatile void* addr)
{
    __raw_writel(b, addr);
}

static inline uint32_t __raw_readl(const volatile void* addr)
{
    return *(const volatile uint32_t*)addr;
}

#define readl(addr) __raw_readl(addr)

typedef struct _rk_handle {
	gestatus (*jtag_status)(bool status)
} rk_handle;

void cur_jtag_func_set(rk_handle* rk_handl);

#endif // __PANGOLIN_RK_COMMON_H__
