/*
 * Peripheral Port Manager (Pangolin) for GDK8 and YourLand Code Book 
 *
 * Copyright (C) 2021-2024 GEGU Tech. Ltd.
*/
#ifndef __GDK_LOG_H__

#define __GDK_LOG_H__

#define GLOG_ERROR	1
#define GLOG_WARN	2
#define GLOG_INFO	3
#define GLOG_DEBUG	4

void log_init(uint8_t dbglevel);
void log_exit(void);
void gdkprint(uint8_t dbglevel, const char* func, const char* fmt, ...);

#endif // __GDK_LOG_H__
