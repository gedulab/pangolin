/*
 * Peripheral Port Manager (Pangolin) for GDK8 and YourLand Code Book 
 *
 * Copyright (C) 2021-2024 GEGU Tech. Ltd.
*/
#include "pangolin.h"
#include "log.h"
#include "rk.h"
#include "rk3588.h"

gestatus rk3588_jtag_status_handle(bool status)
{
    int val_ioc, val_grf, fd;
    void* base_ioc, * base_grf;
    off_t offset, pbase_ioc, pbase_grf, poffset_ioc, poffset_grf, paddr_ioc, paddr_grf;
    size_t len, page_size;

    offset = RK3588_SYSCON_GRF_BASE;
    len = RK3588_SYSCON_GRF_SIZE;
    page_size = sysconf(_SC_PAGE_SIZE);
    pbase_grf = (offset / page_size) * page_size;
    poffset_grf = offset - pbase_grf;
    paddr_grf = poffset_grf + len;

    offset = RK3588_BUS_IOC_BASE;
    len = RK3588_BUS_IOC_GPIO4D_IOMUX_SEL_L;
    page_size = sysconf(_SC_PAGE_SIZE);
    pbase_ioc = (offset / page_size) * page_size;
    poffset_ioc = offset - pbase_ioc;
    paddr_ioc = poffset_ioc + len;

    fd = open("/dev/mem", O_RDWR | O_SYNC);
    base_grf = mmap(NULL, paddr_grf,
        PROT_READ | PROT_WRITE, MAP_SHARED, fd, pbase_grf);
    if (base_grf == NULL) {
        gdkprint(GLOG_INFO, __func__, "failed to map GRF memory at 0x%lx\n", RK3588_SYSCON_GRF_BASE);
        return geunknow;
    }

    base_ioc = mmap(NULL, paddr_ioc,
        PROT_READ | PROT_WRITE, MAP_SHARED, fd, pbase_ioc);
    if (base_grf == NULL) {
        gdkprint(GLOG_INFO, __func__, "failed to map GRF memory at 0x%lx\n", RK3588_BUS_IOC_BASE);
        return geunknow;
    }

    val_grf = readl(base_grf + RK3588_GRF_SOC_CON6);
    gdkprint(GLOG_INFO, __func__, "current value of SOC_CON6 = 0x%x\n", val_grf);
    val_ioc = readl(base_ioc + RK3588_BUS_IOC_GPIO4D_IOMUX_SEL_L);
    gdkprint(GLOG_INFO, __func__, "current value of GPIO4D_IOMUX_SEL_L = 0x%x\n", val_ioc);

    if (status == false) {
        goto TAG_RETURN;
    }

    val_grf = GRF_HIWORD_UPDATE(1, 1, 14);
    gdkprint(GLOG_INFO, __func__, "val_grf is set to = 0x%x\n", val_grf);
    writel(val_grf, base_grf + RK3588_GRF_SOC_CON6);
    val_grf = readl(base_grf + RK3588_GRF_SOC_CON6);
    gdkprint(GLOG_INFO, __func__, "current value of SOC_CON6 = 0x%x\n", val_grf);

    if ((val_ioc & 0x11aa) != 0x11aa) {
        gdkprint(GLOG_INFO, __func__, "will set register status to jtag & uart mode\n");
        // gpio4d0_sel = a
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 0);	    // bit 0 0
        val_ioc |= GRF_HIWORD_UPDATE(1, 1, 1);	    // bit 1 1
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 2);	    // bit 2 0
        val_ioc |= GRF_HIWORD_UPDATE(1, 1, 3);	    // bit 3 1
        // gpio4d1_sel = a
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 4);		// bit 4 0
        val_ioc |= GRF_HIWORD_UPDATE(1, 1, 5);	    // bit 5 1
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 6);	    // bit 6 0
        val_ioc |= GRF_HIWORD_UPDATE(1, 1, 7);	    // bit 7 1
        // gpio4d2_sel = 1
        val_ioc |= GRF_HIWORD_UPDATE(1, 1, 8);      // bit 8  1
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 9);      // bit 9  0
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 10);     // bit 10 0
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 11);     // bit 11 0
        // gpio4d3_sel = 1
        val_ioc |= GRF_HIWORD_UPDATE(1, 1, 12);     // bit 12 1
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 13);     // bit 13 0
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 14);     // bit 14 0
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 15);     // bit 15 0
    }
    else {
		gdkprint(GLOG_INFO, __func__, "will set register status to sd card mode\n");
        // gpio4d0_sel = 0
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 0);	    // bit 0 0
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 1);	    // bit 1 0
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 2);	    // bit 2 0
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 3);	    // bit 3 0
        // gpio4d1_sel = 0
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 4);	    // bit 4 0
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 5);	    // bit 5 0
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 6);	    // bit 6 0
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 7);	    // bit 7 0
        // gpio4d2_sel = 5
        val_ioc |= GRF_HIWORD_UPDATE(1, 1, 8);      // bit 8  1
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 9);      // bit 9  0
        val_ioc |= GRF_HIWORD_UPDATE(1, 1, 10);     // bit 10 1
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 11);     // bit 11 0
        // gpio4d3_sel = 5
        val_ioc |= GRF_HIWORD_UPDATE(1, 1, 12);     // bit 12 1
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 13);     // bit 13 0
        val_ioc |= GRF_HIWORD_UPDATE(1, 1, 14);     // bit 14 1
        val_ioc |= GRF_HIWORD_UPDATE(0, 1, 15);     // bit 15 0
    }

    gdkprint(GLOG_INFO, __func__, "val_ioc is set to = 0x%x\n", val_ioc);
    writel(val_ioc, base_ioc + RK3588_BUS_IOC_GPIO4D_IOMUX_SEL_L);
    val_ioc = readl(base_ioc + RK3588_BUS_IOC_GPIO4D_IOMUX_SEL_L);
    gdkprint(GLOG_INFO, __func__, "current value of GPIO4D_IOMUX_SEL_L = 0x%x\n", val_ioc);

TAG_RETURN:
    munmap(base_grf, paddr_grf);
    munmap(base_ioc, paddr_ioc);
    close(fd);

    if ((val_ioc & 0x11aa) != 0x11aa) {
        return geon;
    }

    return geoff;
}
