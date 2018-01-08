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

typedef struct __message_type_s message_s;

int message_new(message_cmd_e cmd, int servo, int speed, message_s *new_msg);
void message_reset_seq_num(void);

/* To encapsulate message struction,
 * we need to discuss about it.
 * which one is better, hiding its members or direct accessing its members?
 */
int message_get_seq_num(message_s *msg, unsigned long long int *seq_num);
int message_get_cmd(message_s *msg, message_cmd_e *cmd);
int message_get_servo_value(message_s *msg, int *speed);
int message_get_servo_value(message_s *msg, int *servo);

#endif /* __CAR_APP_MESSAGE_H__ */
