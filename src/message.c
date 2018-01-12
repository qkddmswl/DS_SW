/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.1 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <time.h>
#include <limits.h>
#include <stdlib.h>
#include <glib.h>

#include "log.h"
#include "message.h"

static unsigned long long int sequence_number = 0;
static GQueue inqueue = G_QUEUE_INIT;
static GQueue outqueue = G_QUEUE_INIT;

static unsigned long long int __message_get_monotonic_time(void)
{
	unsigned long long int c_time = 0;
	struct timespec ts;
	int ret = 0;

	ret = clock_gettime(CLOCK_MONOTONIC, &ts);
	if (ret)
		_E("failed to get monotonic time");
	else
		c_time = (((unsigned long long int)ts.tv_sec) * 1000000)
			+ (ts.tv_nsec / 1000);

	return c_time;
}

int message_new_to_send(message_cmd_e cmd,
	int servo, int speed, message_s *new_msg)
{
	retv_if(!new_msg, -1);

	new_msg->seq_num = sequence_number++;
	new_msg->cmd = cmd;
	new_msg->servo = servo;
	new_msg->speed = speed;
	new_msg->time = __message_get_monotonic_time();

	if (new_msg->seq_num >= ULLONG_MAX) {
		/* maybe never reach here */
		_W("seq number reachs max value, reset it to 0");
		sequence_number = 0;
	}

	return 0;
}

void message_reset_seq_num(void)
{
	sequence_number = 0;

	return;
}

int message_queue_new(void)
{
	/* Do nothing because we use static queue
	 * if we use multiple thread to handling messages,
	 * message queue should be changed to thread-safe one.
	 */
	return 0;
}

static void __queue_clear_cb(gpointer data, gpointer user_data)
{
	free(data);
	return;
}

void message_queue_clear(void)
{
	g_queue_foreach(&inqueue, __queue_clear_cb, NULL);
	g_queue_clear(&inqueue);

	g_queue_foreach(&outqueue, __queue_clear_cb, NULL);
	g_queue_clear(&outqueue);

	return;
}

void message_push_to_inqueue(message_s *msg)
{
	g_queue_push_tail(&inqueue, msg);
	_D("seq[%llu] is pushed to in-queue", msg->seq_num);
	return;
}

void message_push_to_outqueue(message_s *msg)
{
	g_queue_push_tail(&outqueue, msg);
	_D("seq[%llu] is pushed to out-queue", msg->seq_num);
	return;
}

message_s *message_pop_from_inqueue(void)
{
	return (message_s *)g_queue_pop_head(&inqueue);
}

message_s *message_pop_from_outqueue(void)
{
	return (message_s *)g_queue_pop_head(&outqueue);
}
