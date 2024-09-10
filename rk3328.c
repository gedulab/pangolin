/*
 * Peripheral Port Manager (Pangolin) for GDK8 and YourLand Code Book 
 *
 * Copyright (C) 2021-2024 GEGU Tech. Ltd.
*/
#include "pangolin.h"
#include "log.h"
#include "rk.h"
#include "rk3328.h"

gestatus rk3328_jtag_status_handle(bool status)
{
    int val_grf, fd;
	off_t offset, pbase_ioc, pbase_grf, poffset_ioc, poffset_grf, paddr_ioc, paddr_grf;
    size_t len, page_size;

    offset = RK3328_SYSCON_GRF_BASE;
    len = RK3328_SYSCON_GRF_SIZE;
    page_size = sysconf(_SC_PAGE_SIZE);
    pbase_grf = (offset / page_size) * page_size;
    poffset_grf = offset - pbase_grf;
    paddr_grf = poffset_grf + len;

	offset = RK3328_SYSCON_GRF_BASE;//GD RK3588_BUS_IOC_BASE;
    len = RK3328_GRF_GPIO1A_IOMUX;//GD RK3588_BUS_IOC_GPIO4D_IOMUX_SEL_L;
    page_size = sysconf(_SC_PAGE_SIZE);
    pbase_ioc = (offset / page_size) * page_size;
    poffset_ioc = offset - pbase_ioc;
    paddr_ioc = poffset_ioc + len;

    fd = open("/dev/mem", O_RDWR | O_SYNC);
    pbase_grf = mmap(NULL, poffset_grf + len, 
      PROT_READ | PROT_WRITE, MAP_SHARED, fd, pbase_grf);
    if (pbase_grf == NULL) {
        gdkprint(GLOG_INFO, __func__, "failed to map GRF memory at 0x%lx\n", RK3328_SYSCON_GRF_BASE);
        return -1;
    }

    val_grf = readl(pbase_grf + RK3328_GRF_SOC_CON4);
    gdkprint(GLOG_INFO, __func__, "current value of SOC_CON4 = 0x%x\n", val_grf);

	if (status == false) {
        goto TAG_RETURN;
    }

	val_grf = GRF_HIWORD_UPDATE((status == 0 ? 0 : 1), 1, 12);
	gdkprint(GLOG_INFO, __func__, "val_grf is set to = 0x%x\n", val_grf);
    writel(val_grf, pbase_grf + RK3328_GRF_SOC_CON4);
    val_grf = readl(pbase_grf + RK3328_GRF_SOC_CON4);
    gdkprint(GLOG_INFO, __func__, "current value of SOC_CON4 = 0x%x\n", val_grf);

    if( val_grf & (0x1<<12)) {
        return geon;
    }
    else{
        return geoff;
    }

TAG_RETURN:
    return 0;
}
