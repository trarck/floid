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

#ifndef ANDROID_ACCELO_SENSOR_H
#define ANDROID_ACCELO_SENSOR_H

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include "sensors.h"
#include "SensorBase.h"
#include "InputEventReader.h"

/*****************************************************************************/

struct input_event;

typedef struct {
    short ax;
    short ay;
    short az;
} event_val_t;

class AcceloSensor : public SensorBase {
    event_val_t event_val;
    int mEnabled;
    sensors_event_t mPendingEvent;
    InputEventCircularReader mInputReader;
    bool mHasPendingEvent;
    char input_sysfs_path[PATH_MAX];
    int input_sysfs_path_len;
    int64_t mEnabledTime;

    int setAccelerometerMode(int mode);
    int setInitialState();
    //int calculateRate(int64_t delay_ns);
    int processEvent(input_event const *event, int *count);

public:
    AcceloSensor();
    virtual ~AcceloSensor();
    virtual int readEvents(sensors_event_t* data, int count);
    virtual bool hasPendingEvents() const;
    virtual int setDelay(int32_t handle, int64_t ns);
    virtual int enable(int32_t handle, int enabled);
    virtual int getAttributeFilePath(char file_path[], const char sysfs_path[],
        const char attribute[]);
    int enableAccelerometer();
    int disableAccelerometer();

};

/*****************************************************************************/

#endif  // ANDROID_ACCELO_SENSOR_H
