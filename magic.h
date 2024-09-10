/*
 * Peripheral Port Manager (Pangolin) for GDK8 and YourLand Code Book 
 *
 * Copyright (C) 2021-2024 GEGU Tech. Ltd.
*/
#ifndef __PANGOLIN_MAGIC_H__

#define __PANGOLIN_MAGIC_H__

gestatus otg_status_handle(bool status);
gestatus sysrq_switch_status_handle(bool status);
void sysrq_trigger_status_handle(char* cmd);
void cur_otg_file_set(void);

#endif // __PANGOLIN_MAGIC_H__
