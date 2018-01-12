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


#ifndef __POSITION_FINDER_CONN_MGR_H__
#define __POSITION_FINDER_CONN_MGR_H__

typedef enum {
	CONNECTION_STATE_DISCONNECTED,
	CONNECTION_STATE_CONNECTED,
} connection_state_e;

typedef void(*connection_state_changed_cb)
	(connection_state_e state, const char *ip, void* user_data);

int connection_manager_get_ip(const char **ip);
int connection_manager_init(void);
int connection_manager_fini(void);
int connection_manager_set_state_changed_cb(
	connection_state_changed_cb state_cb, void *user_data);

#endif /* __POSITION_FINDER_CONN_MGR_H__ */
