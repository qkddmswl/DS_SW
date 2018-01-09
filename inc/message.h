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

#ifndef __CAR_APP_MESSAGE_H__
#define __CAR_APP_MESSAGE_H__

typedef enum __message_cmd_e {
	MESSAGE_CMD_HELLO, /* to use keep alive, if needed */
	MESSAGE_CMD_CALIBRATION,
	MESSAGE_CMD_DRIVE,
	MESSAGE_CMD_BYE, /* to notify explicitly closing connection */
} message_cmd_e;

struct __message_type_s {
	unsigned long long int seq_num;
	message_cmd_e cmd;
	int servo;
	int speed;
	unsigned long long int time; /* to be used to order messages */
};
typedef struct __message_type_s message_s;

int message_new(message_cmd_e cmd, int servo, int speed, message_s *new_msg);
void message_reset_seq_num(void);

int message_queue_new(void);
void message_queue_clear(void);

void message_push_to_inqueue(message_s *msg);
message_s *message_pop_from_inqueue(void);

void message_push_to_outqueue(message_s *msg);
message_s *message_pop_from_outqueue(void);

#endif /* __CAR_APP_MESSAGE_H__ */
