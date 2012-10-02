# Copyright (C) 2009 The Android Open Source Project
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
# This file is the build configuration for a full Android
# build for SPEAr1340 hardware. This cleanly combines a set of
# device-specific aspects (drivers) with a device-agnostic
# product configuration (apps).
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base.mk)
# This is where we'd set a backup provider if we had one
#$(call inherit-product, device/sample/products/backup_overlay.mk)

include device/stm/SPEAr1340_10inch/BoardConfig.mk

$(call inherit-product, device/stm/SPEAr1340_10inch/device.mk)

# Discard inherited values and use our own instead.
PRODUCT_NAME := full_SPEAr1340_10inch
PRODUCT_DEVICE := SPEAr1340_10inch
PRODUCT_BRAND := STM
PRODUCT_MODEL := Full Android on SPEAr1340 with 10 inch display