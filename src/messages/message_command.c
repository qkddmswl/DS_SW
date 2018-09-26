/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#ifndef MESSAGE_COMMAND_C
#define MESSAGE_COMMAND_C

#include <string.h>

#include "messages/message_command.h"
#include "messages/macros.h"

static int _message_command_serialze_vcall(message_t *msg, writer_t *writer)
{
	message_command_t *command_msg = container_of(msg, message_command_t, base);
	return message_command_serialize(command_msg, writer);
}

static int _message_command_deserialize_vcall(message_t *msg, reader_t *reader)
{
	message_command_t *command_msg = container_of(msg, message_command_t, base);
	return message_command_deserialize(command_msg, reader);
}

static void _message_command_destroy_vcall(message_t *msg)
{
	message_command_t *command_msg = container_of(msg, message_command_t, base);
	message_command_destroy(command_msg);
}

void message_command_init(message_command_t *message)
{
	message_base_init(&message->base);

	message_set_type(&message->base, MESSAGE_COMMAND);

	message->base.vtable.serialize = _message_command_serialze_vcall;
	message->base.vtable.deserialize = _message_command_deserialize_vcall;
	message->base.vtable.destroy = _message_command_destroy_vcall;

	memset(&message->command, 0x0, sizeof(command_s));
}

void message_command_destroy(message_command_t *message)
{
	message_base_destroy(&message->base);
}

static int _camera_command_deserialize(command_s *cmd, reader_t *reader)
{
	int32_t elevation, azimuth;
	int err = 0;

	err |= reader_read_int32(reader, &elevation);
	err |= reader_read_int32(reader, &azimuth);

	if (err) return err;

	cmd->data.camera_position.camera_elevation = elevation;
	cmd->data.camera_position.camera_azimuth = azimuth;

	return 0;
}

static int _drive_command_deserialize(command_s *cmd, reader_t *reader)
{

	int32_t speed, dir;
	int err = 0;

	err |= reader_read_int32(reader, &speed);
	err |= reader_read_int32(reader, &dir);

	if (err) return err;

	cmd->data.steering.speed = speed;
	cmd->data.steering.direction = dir;

	return 0;
}

static int _drive_and_camera_command_deserialize(command_s *cmd, reader_t *reader)
{

	int32_t speed, dir;
	int32_t elevation, azimuth;
	int err = 0;

	err |= reader_read_int32(reader, &speed);
	err |= reader_read_int32(reader, &dir);
	err |= reader_read_int32(reader, &elevation);
	err |= reader_read_int32(reader, &azimuth);

	if (err) return err;

	cmd->data.steering_and_camera.speed = speed;
	cmd->data.steering_and_camera.direction = dir;
	cmd->data.steering_and_camera.camera_elevation = elevation;
	cmd->data.steering_and_camera.camera_azimuth = azimuth;

	return 0;
}

static int _command_deserialize(command_s *cmd, reader_t *reader)
{
	int32_t type;

	if (reader_read_int32(reader, &type)) {
		return -1;
	}

	cmd->type = type;

	switch (type) {
		case COMMAND_TYPE_DRIVE:
			return _drive_command_deserialize(cmd, reader);
		case COMMAND_TYPE_CAMERA:
			return _camera_command_deserialize(cmd, reader);
			break;
		case COMMAND_TYPE_DRIVE_AND_CAMERA:
			return _drive_and_camera_command_deserialize(cmd, reader);
			break;
		case COMMAND_TYPE_NONE:
			return 0;
		default:
			return -1;
	}
}

static int _camera_command_serialize(command_s *cmd, writer_t *writer)
{
	int32_t elevation = cmd->data.camera_position.camera_elevation;
	int32_t azimuth = cmd->data.camera_position.camera_azimuth;
	int err = 0;

	err |= writer_write_int32(writer, elevation);
	err |= writer_write_int32(writer, azimuth);

	return err;
}

static int _drive_command_serialize(command_s *cmd, writer_t *writer)
{
	int32_t speed = cmd->data.steering.speed;
	int32_t dir = cmd->data.steering.direction;
	int err = 0;

	err |= writer_write_int32(writer, speed);
	err |= writer_write_int32(writer, dir);

	return err;
}

static int _drive_and_camera_command_serialize(command_s *cmd, writer_t *writer)
{
	int32_t speed = cmd->data.steering_and_camera.speed;
	int32_t dir = cmd->data.steering_and_camera.direction;
	int32_t elevation = cmd->data.steering_and_camera.camera_elevation;
	int32_t azimuth = cmd->data.steering_and_camera.camera_azimuth;
	int err = 0;

	err |= writer_write_int32(writer, speed);
	err |= writer_write_int32(writer, dir);
	err |= writer_write_int32(writer, elevation);
	err |= writer_write_int32(writer, azimuth);

	return err;
}

static int _command_serialize(command_s *cmd, writer_t *writer)
{
	int32_t type = cmd->type;

	if (writer_write_int32(writer, type)) {
		return -1;
	}

	switch (cmd->type) {
		case COMMAND_TYPE_DRIVE:
			return _drive_command_serialize(cmd, writer);
		case COMMAND_TYPE_CAMERA:
			return _camera_command_serialize(cmd, writer);
			break;
		case COMMAND_TYPE_DRIVE_AND_CAMERA:
			return _drive_and_camera_command_serialize(cmd, writer);
			break;
		case COMMAND_TYPE_NONE:
			return 0;
		default:
			return -1;
	}
}

int message_command_deserialize(message_command_t *message, reader_t *reader)
{
	int err = 0;

	err |= message_base_deserialize(&message->base, reader);
	err |= _command_deserialize(&message->command, reader);

	return err;
}

int message_command_serialize(message_command_t *message, writer_t *writer)
{
	int err = 0;

	err |= message_base_serialize(&message->base, writer);
	err |= _command_serialize(&message->command, writer);

	return err;
}

const command_s *message_command_get_command(message_command_t *message)
{
	return &message->command;
}

void message_command_set_command(message_command_t *message, const command_s *cmd)
{
	memcpy(&message->command, cmd, sizeof(command_s));
}

#endif /* end of include guard: MESSAGE_COMMAND_C */
