/*
 * Peripheral Port Manager (Pangolin) for GDK8 and YourLand Code Book 
 *
 * Copyright (C) 2021-2024 GEGU Tech. Ltd.
*/
#include "pangolin.h"
#include "log.h"

#include <sys/ipc.h>
#include <sys/msg.h>

#define PANGOLIN_MSGQUE_FILE	"/opt/gedu/pangolin/resources/ipc/pangolin.ipc"
#define GOURDBOY_MSGQUE_FILE	"/opt/gedu/gourdboy/resources/ipc/gourdboy.ipc"
#define NFTS					'c'		// NFTS: 9527
#define WASD					'z'

typedef struct _msgbuf {
	long mtype;
	char mcontext[6];
} msgbuf;

static bool quit;

static int msgque_handle(char FLAG, const char* fname, bool clean)
{
	int msgid;
	key_t key;

	key = ftok(fname, FLAG);
	if (key < 0) {
		gdkprint(GLOG_WARN, __func__, "create key failed with: %d, errno: %d\n", key, errno);
		
		return -1;
	}

	msgid = msgget(key, IPC_CREAT | 0777);
	if (msgid < 0) {
		gdkprint(GLOG_WARN, __func__, "get message queue id failed with: %d, errno: %d\n", msgid, errno);
		
		return -2;
	}

	if (clean == true) {
		gdkprint(GLOG_WARN, __func__, "prepare to clean last message queue\n");
		msgctl(msgid, IPC_RMID, NULL);
	}

	return msgid;
}

static void msg_send(char* data)
{
	int ret, msgid;
	msgbuf msg = { 0 };

	msgid = msgque_handle(WASD, GOURDBOY_MSGQUE_FILE, false);

	msg.mtype = 1;

	snprintf(msg.mcontext, 6, "%s", data);
	ret = msgsnd(msgid, &msg, sizeof(msg), 0);
	if (ret < 0) {
		gdkprint(GLOG_WARN, __func__, "message failed To send\n");

		return;
	}

	gdkprint(GLOG_WARN, __func__, "message send with %s\n", data);

	return;
}

static int msg_rcv(int msgid, msgbuf msg)
{
	int ret;
	int8_t status;
	char data[6];

	ret = msgrcv(msgid, &msg, sizeof(msgbuf), 1, 0);
	if (ret < 0) {
		return -1;
	}

	status = 0;
	gdkprint(GLOG_INFO, __func__, "received message %s\n", msg.mcontext);
	if (strspn(msg.mcontext, "012345678") == strnlen(msg.mcontext, 1)) {
		status = pangolin_parse_cmd(false, msg.mcontext, NULL);
	}
	else if (strspn(msg.mcontext, ZERO2NINE) == strnlen(msg.mcontext, 2)) {
		msg.mcontext[1] = '\0';
		status = pangolin_parse_cmd(true, "mp", msg.mcontext);
	}
	else if (strspn(msg.mcontext, B2Z) == strnlen(msg.mcontext, 1)) {
		status = pangolin_parse_cmd(true, "mp", msg.mcontext);
	}

	snprintf(data, 6, "%d", status);
	msg_send(data);

	return status;
}

static void* server_loop(void* data)
{
	int msgid, ret;
	msgbuf msg = { 0 };

	msgid = msgque_handle(NFTS, PANGOLIN_MSGQUE_FILE, true);
	msgid = msgque_handle(NFTS, PANGOLIN_MSGQUE_FILE, false);

	msg.mtype = 1;

	while (1 == 1) {
		ret = msg_rcv(msgid, msg);
		if (ret < 0) {
			continue;
		}

		if (quit == true) {
			break;
		}
	}

	return NULL;
}

static void server_thread_create(void)
{
	int ret;
	pthread_t p_server;
	void* result;

	ret = pthread_create(&p_server, NULL, server_loop, NULL);
	if (ret != 0) {
		gdkprint(GLOG_WARN, __func__, "create server thread failed with ret: %d\n", ret);

		return;
	}

	ret = pthread_join(p_server, &result);
	if (ret != 0) {
		gdkprint(GLOG_WARN, __func__, "wait for the thread to fail with errno: %d\n", errno);
	
		return;
	}

	return;
}

void server_init(void)
{
	quit = false;
	server_thread_create();

	return;
}

void server_quit(void)
{
	quit = true;

	return;
}
