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

#define LOG_TAG "MagnetoSensor"

#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <cutils/log.h>

#include "MagnetoSensor.h"

#define FETCH_FULL_EVENT_BEFORE_RETURN 1
#define IGNORE_EVENT_TIME 350000000

#define SENSOR_NAME     "lsm303dlh_m"
#define MIN_DELAY       13333333
#define DELAY_75_0HZ    13333333
#define DELAY_30_0HZ    33333333
#define DELAY_15_0HZ    66666667
#define DELAY_7_5HZ     133333333
#define DELAY_3_0HZ     333333333
#define DELAY_1_5HZ     666666667
#define DELAY_0_75HZ    1333333333
#define MAX_DELAY       1333333333

#define SYSFS_PATH_MAG  "/sys/devices/platform/i2c_designware.0/i2c-0/0-001e/"

/*****************************************************************************/

MagnetoSensor::MagnetoSensor()
    : SensorBase(NULL, "magnetometer"),
      mEnabled(0),
      mInputReader(4),
      mHasPendingEvent(false),
      mEnabledTime(0)
{
    mPendingEvent.version = sizeof(sensors_event_t);
    mPendingEvent.sensor = ID_M;
    mPendingEvent.type = SENSOR_TYPE_MAGNETIC_FIELD;
    memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));

    /*if (data_fd) {
        strcpy(input_sysfs_path, "/sys/class/input/");
        strcat(input_sysfs_path, input_name);
        strcat(input_sysfs_path, "/device/device/");
        LOGD("XXXXXXXXXXX Input sysfs path: %s", input_sysfs_path);
        input_sysfs_path_len = strlen(input_sysfs_path);
        enable(0, 1);
    }*/
    enable(0, 1);
}

MagnetoSensor::~MagnetoSensor() {
    if (mEnabled) {
        enable(0, 0);
    }
}

int MagnetoSensor::getAttributeFilePath(char file_path[],
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


int MagnetoSensor::setInitialState() {
    struct input_absinfo absinfo_x;
    struct input_absinfo absinfo_y;
    struct input_absinfo absinfo_z;
    float value;
    if (!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_MAGV_X), &absinfo_x) &&
        !ioctl(data_fd, EVIOCGABS(EVENT_TYPE_MAGV_Y), &absinfo_y) &&
        !ioctl(data_fd, EVIOCGABS(EVENT_TYPE_MAGV_Z), &absinfo_z)) {
        value = absinfo_x.value;
        mPendingEvent.data[0] = value * CONVERT_M_X;
        value = absinfo_x.value;
        mPendingEvent.data[1] = value * CONVERT_M_Y;
        value = absinfo_x.value;
        mPendingEvent.data[2] = value * CONVERT_M_Z;
        mHasPendingEvent = true;
    }
    setDelay(0, MAX_DELAY);
    return 0;
}

int MagnetoSensor::enable(int32_t, int en) {
    int flags = en ? 1 : 0;
    getAttributeFilePath(input_sysfs_path, SYSFS_PATH_MAG, "mode");
    LOGD("XXXXXXXXX Enabling magnetometer %s %d", input_sysfs_path, flags);
    if (flags != mEnabled) {
        int fd;
        fd = open(input_sysfs_path, O_RDWR);
        if (fd >= 0) {
            char buf[2];
            int err;
            buf[1] = 0;
            if (flags) {
                buf[0] = '0';
                mEnabledTime = getTimestamp() + IGNORE_EVENT_TIME;
            } else {
                buf[0] = '3';
            }
            err = write(fd, buf, sizeof(buf));
            close(fd);
            mEnabled = flags;
            if (flags) {
                setInitialState();
            }
            return 0;
        }
        return -1;
    }
    return 0;
}

bool MagnetoSensor::hasPendingEvents() const {
    return mHasPendingEvent;
}

int MagnetoSensor::calculateRate(int64_t delay_ns) {
    if (delay_ns > DELAY_0_75HZ)
        return 0;
    if (delay_ns > DELAY_1_5HZ)
        return 1;
    if (delay_ns > DELAY_3_0HZ)
        return 2;
    if (delay_ns > DELAY_7_5HZ)
        return 3;
    if (delay_ns > DELAY_15_0HZ)
        return 4;
    if (delay_ns > DELAY_30_0HZ)
        return 5;
    return 6;

}

int MagnetoSensor::setDelay(int32_t handle, int64_t delay_ns)
{
    int fd;
    int rate;

    getAttributeFilePath(input_sysfs_path, SYSFS_PATH_MAG, "rate");
    LOGD("XXXXXXXXX magnetometer setting delay %s %lld", input_sysfs_path, delay_ns);

    if (delay_ns < MIN_DELAY)
        return -EINVAL;
    else
        rate = calculateRate(delay_ns);

    fd = open(input_sysfs_path, O_RDWR);
    if (fd >= 0) {
        char buf[3];
        sprintf(buf, "%d", rate);
        LOGD("XXXXXXXXXXXX magnetometer set rate: %d", rate);
        write(fd, buf, strlen(buf)+1);
        close(fd);
        return 0;
    }
    return -1;
}

int MagnetoSensor::readEvents(sensors_event_t* data, int count)
{
    if (count < 1)
        return -EINVAL;

    if (mHasPendingEvent) {
        mHasPendingEvent = false;
        mPendingEvent.timestamp = getTimestamp();
        *data = mPendingEvent;
        return mEnabled ? 1 : 0;
    }

    ssize_t n = mInputReader.fill(data_fd);
    if (n < 0) {
        return n;
    }

    int numEventReceived = 0;
    input_event const* event;

#if FETCH_FULL_EVENT_BEFORE_RETURN
again:
#endif
    while (count && mInputReader.readEvent(&event)) {
        int type = event->type;
        if (type == EV_ABS) {
            float value = event->value;
            if (event->code == ABS_X) {
                mPendingEvent.data[0] = value * CONVERT_M_X;
            } else if (event->code == ABS_Y) {
                mPendingEvent.data[1] = value * CONVERT_M_Y;
            } else if (event->code == ABS_Z) {
                mPendingEvent.data[2] = value * CONVERT_M_Z;
            }
        } else if (type == EV_SYN) {
            mPendingEvent.timestamp = timevalToNano(event->time);
            if (mEnabled) {
                if (mPendingEvent.timestamp >= mEnabledTime) {
                    *data++ = mPendingEvent;
                    numEventReceived++;
                }
                count--;
            }
        } else {
            LOGE("MagnetoSensor: unknown event (type=%d, code=%d)",
                    type, event->code);
        }
        mInputReader.next();
    }

#if FETCH_FULL_EVENT_BEFORE_RETURN
    /* if we didn't read a complete event, see if we can fill and
       try again instead of returning with nothing and redoing poll. */
    if (numEventReceived == 0 && mEnabled == 1) {
        n = mInputReader.fill(data_fd);
        if (n)
            goto again;
    }
#endif
    return numEventReceived;
}

