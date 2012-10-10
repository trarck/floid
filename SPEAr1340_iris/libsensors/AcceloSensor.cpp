/*
 * Copyright (C) 2008 The Android Open Source Project
 * Copyright (C) 2012 Wind River Systems, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "AcceloSensor"

#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <cutils/log.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "AcceloSensor.h"

#define FETCH_FULL_EVENT_BEFORE_RETURN 1
#define IGNORE_EVENT_TIME 350000000

#define SENSOR_NAME     "LIS303DH"
#define MIN_DELAY       1000000
#define DELAY_1000_HZ   1000000
#define DELAY_400_HZ    2500000
#define DELAY_100_HZ    10000000
#define DELAY_50_HZ     20000000
#define MAX_DELAY       20000000
#define MAX_RETRIES     5

#define SYSFS_PATH_ACC  "/sys/class/i2c-adapter/i2c-1/1-0019/"

/*****************************************************************************/
AcceloSensor::AcceloSensor()
    : SensorBase(NULL, "accelerometer"),
      mEnabled(0), mInputReader(4),
      mHasPendingEvent(false),
      mEnabledTime(0)
{
    mPendingEvent.version = sizeof(sensors_event_t);
    mPendingEvent.sensor = ID_A;
    mPendingEvent.type = SENSOR_TYPE_ACCELEROMETER;
    memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));
    /* disable sensor for now, Android will enable it when needed */
    enable(0, 0);
}

AcceloSensor::~AcceloSensor()
{
    if (mEnabled)
        enable(0, 0);
}


int AcceloSensor::getAttributeFilePath(char file_path[],
                    const char sysfs_path[],
                    const char attribute[])
{
    int len = strlen(sysfs_path) + strlen(attribute);
    int ret = 0;

    if (!file_path)
        return -EINVAL;

    if (len >= PATH_MAX)
        return -EINVAL;

    ret = snprintf(file_path, len + 1, "%s%s", sysfs_path, attribute);
    if (ret != len) {
        LOGE("%s: Error building filepath %s\n", SENSOR_NAME,
            file_path);
        memset(file_path, 0, PATH_MAX);
        return -EINVAL;
    }

    return ret;
}

int AcceloSensor::setAccelerometerMode(int mode)
{
    char buf[10];
    int err;
    int fd;

    getAttributeFilePath(input_sysfs_path, SYSFS_PATH_ACC, "mode");
    LOGD("Setting ACCELOROMETER %s mode %d", input_sysfs_path, mode);

    fd = open(input_sysfs_path, O_RDWR);
    if (fd < 0) {
        LOGE("%s: can't open file %s, %s\n",
             SENSOR_NAME, input_sysfs_path, strerror(errno));
        return -1;
    }

    sprintf(buf, "%d", mode);
    err = write(fd, buf, strlen(buf) + 1);
    if (err < 0) {
        LOGE("%s: can't write %s to file %s, %s\n",
             SENSOR_NAME, buf, input_sysfs_path, strerror(errno));
        close(fd);
        return -1;
    }

    close(fd);
    mEnabled = mode;
    return 0;
}

int AcceloSensor::enableAccelerometer()
{
    return setAccelerometerMode(1);
}

int AcceloSensor::disableAccelerometer()
{
    return setAccelerometerMode(0);
}

int AcceloSensor::setInitialState()
{
    mPendingEvent.data[0] = 0;
    mPendingEvent.data[1] = 0;
    mPendingEvent.data[2] = 0;
    return enableAccelerometer();
}

int AcceloSensor::enable(int32_t, int en)
{
    int ret = 0;

    if (mEnabled == en)
        return 0;

    mEnabled = en;
    if (mEnabled)
        ret = setInitialState();
    else
        ret = disableAccelerometer();

    if (ret < 0)
        mEnabled = 0;

    return ret;
}

bool AcceloSensor::hasPendingEvents() const
{
    return mHasPendingEvent;
}

int AcceloSensor::setDelay(int32_t handle, int64_t delay_ns)
{
    char buf[10];
    int fd;
    int rate;
    int err;

    getAttributeFilePath(input_sysfs_path, SYSFS_PATH_ACC, "delay");

    if (delay_ns < MIN_DELAY)
        return -EINVAL;
    else
        rate = delay_ns / 1000000;

    LOGD("accelometer setting delay %s %d", input_sysfs_path, rate);

    fd = open(input_sysfs_path, O_RDWR);
    if (fd < 0) {
        LOGE("%s: can't open file %s, %s\n",
             SENSOR_NAME, input_sysfs_path, strerror(errno));
        return -1;
    }

    sprintf(buf, "%d", rate);
    err = write(fd, buf, strlen(buf) + 1);
    if (err < 0) {
        LOGE("%s: can't write %s to file %s, %s\n",
             SENSOR_NAME, buf, input_sysfs_path, strerror(errno));
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int AcceloSensor::processEvent(input_event const *event, int *count)
{
	int numEventReceived = 0;
	while ((*count) && mInputReader.readEvent(&event)) {
		switch (event->type) {
		case EV_ABS:
			switch (event->code) {
			case EVENT_TYPE_ACCEL_X:
				event_val.ax = event->value;
				break;
			case EVENT_TYPE_ACCEL_Y:
				event_val.ay = event->value;
				break;
			case EVENT_TYPE_ACCEL_Z:
				event_val.az = event->value;
				break;
			default:
				LOGE("%s: Unknown event code (code=%d)",
				     SENSOR_NAME, event->code);
				break;
			};
			break;
		case EV_SYN:
			mPendingEvent.data[0] = event_val.ax * CONVERT_A_X;
			mPendingEvent.data[1] = event_val.ay * CONVERT_A_Y;
			mPendingEvent.data[2] = event_val.az * CONVERT_A_Z;
			numEventReceived++;
			(*count)--;
			break;
		default:
			LOGE("%s: Unknown event (type=%d, code=%d)",
			     SENSOR_NAME, event->type, event->code);
			break;
		}
		mInputReader.next();
	}

	return numEventReceived;
}

int AcceloSensor::readEvents(sensors_event_t * data, int count)
{
	int ret;
	int numEventReceived = 0;
	input_event const *event;
	int max_retries = MAX_RETRIES;

	if (count < 1)
		return -EINVAL;

	if (!data) {
		LOGE("%s: sensor event system compromized \n", SENSOR_NAME);
		return -EINVAL;
	}

	if (mHasPendingEvent) {
		mHasPendingEvent = false;
		mPendingEvent.timestamp = getTimestamp();
		*data = mPendingEvent;
		return mEnabled ? 1 : 0;
	}

	/* if we didn't read a complete event, see if we can fill and
	   try again instead of returning with nothing and redoing poll. */
	while ((numEventReceived == 0) && (max_retries > 0)) {
		ret = mInputReader.fill(data_fd);
		if (ret)
			numEventReceived = processEvent(event, &count);
		else
			LOGE("%s: Unable to read device data \n", SENSOR_NAME);
		max_retries--;
	}

	/* We have a complete event to send to upper layer */
	if (numEventReceived && mEnabled) {
		/* This event is passed to upper layers so set the current time */
		mPendingEvent.timestamp = getTimestamp();
		/* Pass only current event discarding the rest of them */
		/* Discarding late events greatly improve performance */
		*data = mPendingEvent;
	}

	return numEventReceived;
}
