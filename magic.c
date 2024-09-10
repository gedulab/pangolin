/*
 * Peripheral Port Manager (Pangolin) for GDK8 and YourLand Code Book 
 *
 * Copyright (C) 2021-2024 GEGU Tech. Ltd.
*/
#include "pangolin.h"
#include "log.h"

#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>

#define SYSRQ_TRIGGER_FILE		"/proc/sysrq-trigger"
#define SYSRQ_SWITCH_FILE		"/proc/sys/kernel/sysrq"
#define ULAN_OTG_FILE			"/sys/devices/platform/fd5d0000.syscon/fd5d0000.syscon:usb2-phy@0/otg_mode"
#define GDK8_OTG_FILE			"/sys/devices/platform/ff450000.syscon/ff450000.syscon:usb2-phy@100/otg_mode"

static char cur_otg_file[PATH_MAX] = { 0 };

typedef struct _file_context_info {
	ssize_t len;
	char rbuf[PATH_MAX];
} file_context_info;

typedef struct _file_operate {
	int64_t (*gfopen)(char* tmp);
	file_context_info (*gfread)(int64_t fd);
	file_context_info (*gfwrite)(int64_t fd, char* cmd);
	void (*gfclose)(int64_t fd);
} file_operate;

static int64_t file_open(char* tmp)
{
	int64_t fd;
	char fpath[PATH_MAX];

	gdkprint(GLOG_INFO, __func__, "prepare to open file with %s\n", tmp);


	strncpy(fpath, tmp, PATH_MAX);

	fd = open(fpath, O_RDWR);
	if (fd < 0) {
		gdkprint(GLOG_WARN, __func__, "failed to open file %s with errno: %d\n", fpath, errno);
	}

	return fd;
}

static file_context_info file_read(int64_t fd)
{
	file_context_info tmp;

	gdkprint(GLOG_INFO, __func__, "prepare to read file with fd: %ld\n", fd);

	if (fd < 0) {
		gdkprint(GLOG_WARN, __func__, "bad file descriptor\n");
		tmp.len = -1;

		return tmp;
	}

	tmp.len = read(fd, tmp.rbuf, PATH_MAX);
	if (tmp.len < 0) {
		gdkprint(GLOG_WARN, __func__, "failed to read file with errno: %d\n", errno);

		return tmp;
	}
	tmp.rbuf[tmp.len] = '\0';

	return tmp;
}

static file_context_info file_write(int64_t fd, char* cmd)
{
	file_context_info tmp;

	gdkprint(GLOG_INFO, __func__, "prepare to write file with fd: %ld\n", fd);

	if (fd < 0) {
		gdkprint(GLOG_WARN, __func__, "bad file descriptor\n");
		tmp.len = -1;

		return tmp;
	}

	tmp.len = write(fd, cmd, strlen(cmd));
	if (tmp.len < 0) {
		gdkprint(GLOG_WARN, __func__, "failed to write file with cmd: %s, errno: %d\n", cmd, errno);

		return tmp;
	}

	snprintf(tmp.rbuf, PATH_MAX, "%s", cmd);
	gdkprint(GLOG_WARN, __func__, "set the status to %s success\n", cmd);

	return tmp;
}

static void file_close(int64_t fd)
{
	gdkprint(GLOG_INFO, __func__, "prepare to close file with fd: %ld\n", fd);

	close(fd);

	return;
}

static file_operate foperate = {
	.gfopen = file_open,
	.gfread = file_read,
	.gfwrite = file_write,
	.gfclose = file_close
};

gestatus otg_status_handle(bool status)
{
	uint8_t site;
	int64_t fd;
	file_context_info fcinfo;
	char* cmd[] = {
		"otg", "host"
	};
	
	fd = foperate.gfopen(cur_otg_file);
	fcinfo = foperate.gfread(fd);
	if (fcinfo.len < 0) {
		return geunknow;
	}

	if (status == false) {
		goto TAG_RETURN;
	}

	site = strncmp(fcinfo.rbuf, "host", 4) == 0 ? 0 : 1;
	fcinfo = foperate.gfwrite(fd, cmd[site]);

TAG_RETURN:
	foperate.gfclose(fd);

	return (strncmp(fcinfo.rbuf, "otg", 4) == 0 ? geon : geoff);
}

void cur_otg_file_set(void) {
	switch (cur_machine) {
	case is_gdk8:
		strncpy(cur_otg_file, GDK8_OTG_FILE, PATH_MAX);
		break;
	case is_ulan:
		strncpy(cur_otg_file, ULAN_OTG_FILE, PATH_MAX);
		break;
	default:
		gdkprint(GLOG_INFO, __func__, "unsupported machine, will exit\n");
		pangolin_clean_up(UNSUPPORT_MACHINE);
		break;
	}

	return;
}

gestatus sysrq_switch_status_handle(bool status)
{
	uint8_t site;
	int64_t fd;
	file_context_info fcinfo;
	char* cmd[] = {
		"1", "0"
	};

	fd = foperate.gfopen(SYSRQ_SWITCH_FILE);
	fcinfo = foperate.gfread(fd);
	if (fcinfo.len < 0) {
		return geunknow;
	}

	if (status == false) {
		goto TAG_RETURN;
	}

	site = strncmp(fcinfo.rbuf, "0", 2) == 0 ? 0 : 1;
	fcinfo = foperate.gfwrite(fd, cmd[site]);

TAG_RETURN:
	foperate.gfclose(fd);

	return (strncmp(fcinfo.rbuf, "0", 2) == 0 ? geoff : geon);
}

void sysrq_trigger_status_handle(char* cmd)
{
	int64_t fd;

	fd = foperate.gfopen(SYSRQ_TRIGGER_FILE);

	foperate.gfwrite(fd, cmd);

	foperate.gfclose(fd);

	return;
}
