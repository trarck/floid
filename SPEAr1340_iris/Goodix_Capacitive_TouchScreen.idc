# Copyright (C) 2010 The Android Open Source Project
# Copyright (C) 2012 Wind River Systems, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# Input Device Calibration File for the eGalax USB touch controller
#

# Basic Parameters
touch.deviceType = touchScreen
touch.orientationAware = 1
device.internal = 1

# Size
# Based on empirical measurements, we estimate the size of the contact
# using size = sqrt(area) * 43 + 0.
touch.size.calibration = area
touch.size.scale = 43
touch.size.bias = 0
touch.size.isSummed = 0

# Pressure
# Driver reports signal strength as pressure.
#
# A normal thumb touch typically registers about 80 signal strength
# units although we don't expect these values to be accurate.
touch.pressure.calibration = default
touch.pressure.scale = 1.0

# Orientation
touch.orientation.calibration = none

