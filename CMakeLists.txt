CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(${P_NAME} C)

SET(INSTALL_EXEC_PREFIX "${INSTALL_PREFIX}/bin")
SET(CMAKE_VERBOSE_MAKEFILE 0)

SET(PROJECT_ROOT_DIR "${CMAKE_SOURCE_DIR}")
SET(PROJECT_RESOURCES_DIR "${PROJECT_ROOT_DIR}/res")

INCLUDE(FindPkgConfig)
pkg_check_modules(APP_PKGS REQUIRED
	dlog
	capi-appfw-application
	capi-appfw-service-application
	capi-system-peripheral-io
	glib-2.0
	gio-2.0
	capi-network-connection
)

FOREACH (flag ${APP_PKGS_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} -fvisibility=hidden -Wall -Winline -g -fno-builtin-malloc -fPIE")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -pie")

INCLUDE_DIRECTORIES(${PROJECT_ROOT_DIR}/inc)

ADD_EXECUTABLE(${PROJECT_NAME}
	${PROJECT_ROOT_DIR}/src/app.c
	${PROJECT_ROOT_DIR}/src/log.c
	${PROJECT_ROOT_DIR}/src/connection_manager.c
	${PROJECT_ROOT_DIR}/src/resource.c
	${PROJECT_ROOT_DIR}/src/message.c
	${PROJECT_ROOT_DIR}/src/receiver.c
	${PROJECT_ROOT_DIR}/src/receiver_udp.c
	${PROJECT_ROOT_DIR}/src/resource/resource_infrared_obstacle_avoidance_sensor.c
	${PROJECT_ROOT_DIR}/src/resource/resource_motor_driver_L298N.c
	${PROJECT_ROOT_DIR}/src/resource/resource_PCA9685.c
	${PROJECT_ROOT_DIR}/src/resource/resource_servo_motor.c
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_LDFLAGS} -lm)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${APP_PKGS_LDFLAGS})

CONFIGURE_FILE(${PROJECT_ROOT_DIR}/tizen-manifest.xml.in ${ORG_PREFIX}.${PROJECT_NAME}.xml @ONLY)
# Install
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_EXEC_PREFIX})
INSTALL(FILES ${ORG_PREFIX}.${PROJECT_NAME}.xml DESTINATION ${SYS_PACKAGES_DIR})
INSTALL(FILES ${PROJECT_ROOT_DIR}/shared/res/default_icon.png DESTINATION ${SYS_ICONS_DIR} RENAME ${PROJECT_NAME}.png)

# End of a file