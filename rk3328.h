/*
 * Peripheral Port Manager (Pangolin) for GDK8 and YourLand Code Book 
 *
 * Copyright (C) 2021-2024 GEGU Tech. Ltd.
*/
#ifndef __PANGOLIN_RK3328_H__

#define RK3328_SYSCON_GRF_BASE		0xff100000
#define RK3328_SYSCON_GRF_SIZE		0x1000
#define RK3328_GRF_SOC_CON4			0x410
#define RK3328_GRF_GPIO1A_IOMUX		0x10

gestatus rk3328_jtag_status_handle(bool status);

#endif // __PANGOLIN_RK3328_H__
