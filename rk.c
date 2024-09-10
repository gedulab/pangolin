/*
 * Peripheral Port Manager (Pangolin) for GDK8 and YourLand Code Book 
 *
 * Copyright (C) 2021-2024 GEGU Tech. Ltd.
*/
#include "pangolin.h"
#include "log.h"
#include "rk.h"
#include "rk3588.h"
#include "rk3328.h"

void cur_jtag_func_set(rk_handle* rk_handl)
{
	switch (cur_machine) {
	case is_gdk8:
		rk_handl->jtag_status = rk3328_jtag_status_handle;
		break;
	case is_ulan:
		rk_handl->jtag_status = rk3588_jtag_status_handle;
		break;
	default:
		gdkprint(GLOG_INFO, __func__, "unsupported machine, will exit\n");
		pangolin_clean_up(UNSUPPORT_MACHINE);
		break;
	}

	return;
}
