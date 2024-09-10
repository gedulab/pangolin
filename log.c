/*
 * Peripheral Port Manager (Pangolin) for GDK8 and YourLand Code Book 
 *
 * Copyright (C) 2021-2024 GEGU Tech. Ltd.
*/
#include "pangolin.h"
#include "log.h"

#include <syslog.h>
#include <fcntl.h>
#include <time.h>
#include <stdarg.h>

#define LOG_PATH_MAX	48
#define LOG_PRINT_MAX	1024

typedef struct _gdk_log_info {
	int64_t fd;
	uint8_t curlevel;
	bool has_init;
} gdk_log_info;

static gdk_log_info glog;

static char* log_path_base[] = {
	"/opt/gedu/logs/",	// gdk
	"/var/log/",		// linux
	"/data/",			// android
};

static char* log_level_tag[] = {
	"[EE]",	// error
	"[WW]",	// warn
	"[II]",	// info
	"[DD]"	// debug
};

void gdkprint(uint8_t dbglevel, const char* func, const char* fmt, ...)
{
	char tmp[LOG_PRINT_MAX], buf[LOG_PRINT_MAX + LOG_PATH_MAX];
	va_list ptr;
	time_t timeval;
	struct tm* curtime;

	if (dbglevel > glog.curlevel && glog.has_init == true) {
		return;
	}

	if ((dbglevel < 1 || dbglevel > 4) && glog.has_init == true) {
		syslog(LOG_LOCAL6 | LOG_WARNING, "%s: need the right level of debugging [1 - 4]\n", __func__);

		return;
	}

	va_start(ptr, fmt);
	vsnprintf(tmp, LOG_PRINT_MAX, fmt, ptr);
	va_end(ptr);

	if (glog.has_init == false) {
		fprintf(stderr, "%s", tmp);

		return;
	}

	time(&timeval);
	curtime = gmtime(&timeval);

	snprintf(buf, strnlen(tmp, LOG_PRINT_MAX) + LOG_PATH_MAX, "%04d-%02d%02d-%02d:%02d:%02d %s: %s -> %s",
		1900 + curtime->tm_year, 1 + curtime->tm_mon, curtime->tm_mday,
		8 + curtime->tm_hour, curtime->tm_min, curtime->tm_sec,
		log_level_tag[dbglevel - 1], func, tmp);

	if (write(glog.fd, buf, strnlen(buf, LOG_PRINT_MAX + LOG_PATH_MAX)) < 0) {
		syslog(LOG_LOCAL6 | LOG_WARNING, "%s: failed to write log file, content: %s -> %s, errno: %d\n",
			__func__, func, buf, errno);
	}

	return;
}

static void log_file_open(void)
{
	uint8_t i;
	char buf[LOG_PATH_MAX];

	glog.has_init = false;

	for (i = 0; i < (sizeof(log_path_base) / sizeof(log_path_base[0])); i++) {
		snprintf(buf, LOG_PATH_MAX, "%s%s0.log", log_path_base[i], PROGRAM_NAME);

		glog.fd = open(buf, O_WRONLY | O_TRUNC | O_CREAT, 0644);
		if (glog.fd < 0) {
			syslog(LOG_LOCAL6 | LOG_WARNING, "%s: log file failed to open with path: %s\n", __func__, buf);

			if (i == ((sizeof(log_path_base) / sizeof(log_path_base[0]) - 1))) {
				syslog(LOG_LOCAL6 | LOG_ERR, "%s: all log files have failed to be created and the program will exit, errno: %d\n", __func__, errno);
				exit(LOG_OPEN_FAILED);
			}
		}
		else {
			syslog(LOG_LOCAL6 | LOG_INFO, "%s: log file open successfully with path: %s\n", __func__, buf);
			break;
		}
	}

	snprintf(buf, LOG_PATH_MAX, "try to write file, current level is: %d!\n", glog.curlevel);
	if (write(glog.fd, buf, strnlen(buf, LOG_PATH_MAX)) < 0) {
		pangolin_clean_up(ATTEMPT_WRITE_FAILED);
	}

	glog.has_init = true;

	return;
}

void log_init(uint8_t dbglevel)
{
	syslog(LOG_LOCAL6 | LOG_INFO, "%s: ready to init log\n", __func__);

	glog.curlevel = dbglevel;
	log_file_open();

	return;
}

void log_exit(void)
{
	if (glog.has_init == false) {
		return;
	}

	gdkprint(GLOG_INFO, __func__, "prepare to clean up the log\n");

	close(glog.fd);

	return;
}
