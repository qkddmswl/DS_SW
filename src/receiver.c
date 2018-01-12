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

#include <stdlib.h>
#include "log.h"
#include "receiver_internal.h"

typedef struct __receiver_h {
	receiver_type_e receiver_type;
	/* TODO */
} receiver_h;

static receiver_h *ghandle = NULL;

int receiver_init(receiver_type_e type)
{
	int ret = 0;

	if (ghandle) {
		_E("receiver is already initialized");
		return -1;
	}

	ghandle = (receiver_h *)malloc(sizeof(receiver_h));
	if (!ghandle) {
		_E("failed to malloc handle");
		return -1;
	}

	switch (type) {
	case RECEIVER_TYPE_UDP:
		ghandle->receiver_type = type;
		ret = receiver_udp_start();
		if (ret)
			goto ERROR;
		break;
	case RECEIVER_TYPE_BLUETOOTH:
		/* TODO */
		break;
	}
	return 0;

ERROR:
	free(ghandle);
	ghandle = NULL;
	return -1;
}

void receiver_fini(void)
{
	if (ghandle) {
		switch (ghandle->receiver_type) {
		case RECEIVER_TYPE_UDP:
			receiver_udp_stop();
			break;
		case RECEIVER_TYPE_BLUETOOTH:
			/* TODO */
			break;
		}

		free(ghandle);
		ghandle = NULL;
	}
	return;
}
