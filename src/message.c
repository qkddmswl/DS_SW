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
#include "log.h"
#include "message.h"

struct __message_type_s {
	unsigned long long int seq_num;
	message_cmd_e cmd;
	int servo;
	int speed;
	unsigned long long int time; /* to be used to order messages */
};

static unsigned long long int sequence_number = 0;

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

int message_new(message_cmd_e cmd, int servo, int speed, message_s *new_msg)
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

int message_get_seq_num(message_s *msg, unsigned long long int *seq_num)
{
	retv_if(!msg, -1);

	*seq_num = msg->seq_num;

	return 0;
}

int message_get_cmd(message_s *msg, message_cmd_e *cmd)
{
	retv_if(!msg, -1);

	*cmd = msg->cmd;

	return 0;
}

int message_get_speed_value(message_s *msg, int *speed)
{
	retv_if(!msg, -1);

	*speed = msg->speed;

	return 0;
}

int message_get_servo_value(message_s *msg, int *servo)
{
	retv_if(!msg, -1);

	*servo = msg->servo;

	return 0;
}
