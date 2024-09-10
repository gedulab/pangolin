/*
 * Peripheral Port Manager (Pangolin) for GDK8 and YourLand Code Book 
 *
 * Copyright (C) 2021-2024 GEGU Tech. Ltd.
*/
#include "pangolin.h"
#include "log.h"
#include "magic.h"
#include "rk.h"
#include "ipc.h"

#include <signal.h>
#include <ctype.h>

gemachine cur_machine = other_machine;
rk_handle cur_rk_handle = {};

void pangolin_clean_up(const int exitnum)
{
	gdkprint(GLOG_INFO, __func__, "the program will exit with exitno: %d\n", exitnum);
	
	server_quit();
	log_exit();

	exit(exitnum);
}

static void signal_handle(int signum)
{
	gdkprint(GLOG_INFO, __func__, "receive SIGTERM or SIGTERM, the program will exit, signo: %d\n", signum);

	pangolin_clean_up(signum);
}

static void usage(void)
{
	gdkprint(GLOG_INFO, __func__, "---------------------------------------------------------------------\n");
	gdkprint(GLOG_INFO, __func__, "| usage: sudo pangolin [mp] {sysrq trigger}                         |\n");
	gdkprint(GLOG_INFO, __func__, "---------------------------------------------------------------------\n");
	gdkprint(GLOG_INFO, __func__, "| mp (mana point) introduce:                                        |\n");
	gdkprint(GLOG_INFO, __func__, "|     use features by setting the mp bits, or ask for help          |\n");
	gdkprint(GLOG_INFO, __func__, "| usage: -h / -? / --help                                           |\n");
	gdkprint(GLOG_INFO, __func__, "| bit0:  otg   toggle -> 1  | original -> 0                         |\n");
	gdkprint(GLOG_INFO, __func__, "| bit1:  jtag  toggle -> 2  | original -> 0                         |\n");
	gdkprint(GLOG_INFO, __func__, "| bit2:  sysrq toggle -> 4  | original -> 0                         |\n");
	gdkprint(GLOG_INFO, __func__, "---------------------------------------------------------------------\n");
	gdkprint(GLOG_INFO, __func__, "| muggle introduce:                                                 |\n");
	gdkprint(GLOG_INFO, __func__, "|     syrsrq is a magic that turns into a muggle when used          |\n");
	gdkprint(GLOG_INFO, __func__, "|     https://www.kernel.org/doc/html/latest/admin-guide/sysrq.html |\n");
	gdkprint(GLOG_INFO, __func__, "| b - z: use the command sysrq                                      |\n");
	gdkprint(GLOG_INFO, __func__, "| 0 - 9: set the log level                                          |\n");
	gdkprint(GLOG_INFO, __func__, "| Example:                                                          |\n");
	gdkprint(GLOG_INFO, __func__, "| If you want to set otg，Please run command:                       |\n");
	gdkprint(GLOG_INFO, __func__, "| sudo ./pangolin 1                                                 |\n");
	gdkprint(GLOG_INFO, __func__, "| If you want to set jtag，Please run command:                      |\n");
	gdkprint(GLOG_INFO, __func__, "| sudo ./pangolin 2                                                 |\n");
	gdkprint(GLOG_INFO, __func__, "| If you want to set otg and jtag，Please run command:              |\n");
	gdkprint(GLOG_INFO, __func__, "| sudo ./pangolin 3                                                 |\n");
	gdkprint(GLOG_INFO, __func__, "---------------------------------------------------------------------\n");

	pangolin_clean_up(USAGE_VIEW);
}

static bool bit_check(uint8_t mp, uint8_t bit)
{
	if ((mp & bit) == bit) {
		return true;
	}

	return false;
}

static int8_t returan_value_handle(gestatus ret, uint8_t status)
{
	switch (ret) {
	case geon:
		return status;
	case geoff:
		return 0;
	default:
		return 0;
	}
}

static int8_t otg_status_prepare2change(bool status)
{
	gestatus ret;

	gdkprint(GLOG_INFO, __func__, "whether switch otg status: %s\n", status ? "true " : "false");

	ret = otg_status_handle(status);

	return returan_value_handle(ret, 1);
}

static int8_t jtag_status_prepare2change(bool status)
{
	gestatus ret;

	gdkprint(GLOG_INFO, __func__, "whether switch jtag status: %s\n", status ? "true " : "false");

	if (cur_rk_handle.jtag_status) {
		ret = cur_rk_handle.jtag_status(status);
	}
	else {
		ret = geunknow;
	}

	return returan_value_handle(ret, 2);
}

static int8_t sysrq_status_prepare2change(bool status)
{
	gestatus ret;

	gdkprint(GLOG_INFO, __func__, "whether switch sysrq status: %s\n", status ? "true " : "false");

	ret = sysrq_switch_status_handle(status);

	return returan_value_handle(ret, 4);
}

static int8_t pangolin_use_cmd(uint8_t mp)
{
	int8_t status;

	status = 0;
	status += otg_status_prepare2change(bit_check(mp, 1));
	status += jtag_status_prepare2change(bit_check(mp, 2));
	status += sysrq_status_prepare2change(bit_check(mp, 4));

	return status;
}

int8_t pangolin_parse_cmd(bool muggle_mode, char* mp, char* muggle)
{
	int8_t status;
	uint8_t mpvalue;

	status = 0;
	switch (muggle_mode) {
	case false:
		if (strspn(mp, ZERO2NINE) == strnlen(mp, 1)) {
			sscanf(mp, "%hhd", &mpvalue);
			status = pangolin_use_cmd(mpvalue);
		}
		else if ((strncmp(mp, "-h", 2) == 0) || (strncmp(mp, "-?", 2) == 0) || (strncmp(mp, "--help", 6) == 0)) {
			usage();
		}
		else {
			gdkprint(GLOG_INFO, __func__, "mp of pangolin is not used correctly\n");
		}
		break;
	case true:
		if (strncmp(mp, "mp", 3) != 0) {
			usage();
		}
		sysrq_trigger_status_handle(muggle);
		break;
	}

	return status;
}

static void args_parse(int argc, char* argv[])
{
	switch (argc) {
	case 2:
		pangolin_parse_cmd(false, argv[1], NULL);
		goto TAG_QUIT;
	case 3:
		if (strspn(argv[2], ZERO2NINE) == strnlen(argv[2], 1) || strspn(argv[2], B2Z) == strnlen(argv[2], 1)) {
			pangolin_parse_cmd(true, argv[1], argv[2]);
			
		}
		else {
			usage();
		}
		goto TAG_QUIT;
	default:
		break;
	}

	return;

TAG_QUIT:
	pangolin_clean_up(USE_PANGOLIN_CMD);
}

static gemachine env_check(void) {
	gemachine curm;

	curm = is_gdk8;

	return curm;
}

int main(int argc, char* argv[])
{
	uint8_t dbglevel;

	signal(SIGTERM, signal_handle);
	signal(SIGINT, signal_handle);
	signal(SIGPIPE, SIG_IGN);
	if(argc > 1 && strcmp(argv[1], "-h") == 0)
	{
		usage();
	}
	dbglevel = 4;
	log_init(dbglevel);

	gdkprint(GLOG_INFO, __func__, "log initialization complete, %s v%d.%d, pid: %d\n",
		PROGRAM_NAME, MAJOR_VERSION, MINOR_VERSION, getpid());

	cur_machine = env_check();
	if (cur_machine == other_machine) {
		gdkprint(GLOG_INFO, __func__, "unsupported machine, will exit\n");
		pangolin_clean_up(UNSUPPORT_MACHINE);
	}

	cur_otg_file_set();
	cur_jtag_func_set(&cur_rk_handle);

	args_parse(argc, argv);
	if(argc < 2){
	server_init();
	}
	pangolin_clean_up(0);
}
