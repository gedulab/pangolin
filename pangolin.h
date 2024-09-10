/*
 * Peripheral Port Manager (Pangolin) for GDK8 and YourLand Code Book 
 *
 * Copyright (C) 2021-2024 GEGU Tech. Ltd.
*/
#ifndef __PANGOLIN_COMMON_H__

#define __PANGOLIN_COMMON_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#define _GNU_SOURCE
#define __USE_GNU
#include <sched.h>
#include <pthread.h>

#define PROGRAM_NAME				"pangolin"
#define PROGRAM_DESC				"GeDu-Domain"
#define MAJOR_VERSION				3
#define MINOR_VERSION				3

#define USE_PANGOLIN_CMD			2
#define USAGE_VIEW					1
#define LOG_OPEN_FAILED				-1
#define ATTEMPT_WRITE_FAILED		-2
#define UNSUPPORT_MACHINE			-3

#define ZERO2NINE	"0123456789"
#define B2Z			"bcdefghijklmnopqrstuvwxyz"

typedef enum _gestatus {
	geunknow = -1,
	geoff,
	geon
} gestatus;

typedef enum _gemachine {
	other_machine = -1,
	is_gdk8,
	is_ulan
} gemachine;

extern gemachine cur_machine;

void pangolin_clean_up(const int exitnum);
int8_t pangolin_parse_cmd(bool muggle_mode, char* mp, char* muggle);

#endif // __PANGOLIN_COMMON_H__
