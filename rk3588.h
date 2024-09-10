/*
 * Peripheral Port Manager (Pangolin) for GDK8 and YourLand Code Book 
 *
 * Copyright (C) 2021-2024 GEGU Tech. Ltd.
*/
#ifndef __PANGOLIN_RK3588_H__

#define RK3588_SYSCON_GRF_BASE					0xfd58c000
#define RK3588_SYSCON_GRF_SIZE					0x1000
#define RK3588_GRF_SOC_CON6						0x0318
#define RK3588_BUS_IOC_BASE						0xfd5f8000
#define RK3588_BUS_IOC_GPIO4D_IOMUX_SEL_L		0x0098

gestatus rk3588_jtag_status_handle(bool status);

#endif // __PANGOLIN_RK3588_H__
