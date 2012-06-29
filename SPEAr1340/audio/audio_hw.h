/*
 * Copyright (C) 2011 The Android Open Source Project
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
#ifndef AUDIO_HW_H
#define AUDIO_HW_H

/* Mixer control names */

#define MIXER_MASTER_PLAYBACK_VOLUME        "Master Playback Volume"
#define MIXER_LEFT_PLAYBACK_VOLUME          "Left Playback Volume"
#define MIXER_RIGHT_PLAYBACK_VOLUME         "Right Playback Volume"
#define MIXER_MASTER_MUTE                   "master mute"

#define CARD_SPEAR1340_EVB 0
#define CARD_SPEAR_DEFAULT CARD_SPEAR1340_EVB

/* ALSA ports for SPEAr1340 EVB */
#define SPDIF_PLAYBACK 0
#define SPDIF_RECORD   1
#define I2S_PLAYBACK   2
#define I2S_RECORD     3

/* number of frames per long period (low power) */
#define LONG_PERIOD_SIZE 512
/* number of periods for low power playback */
#define PLAYBACK_LONG_PERIOD_COUNT 8

/* number of frames per short period (low latency) */
#define SHORT_PERIOD_SIZE 512
/* number of pseudo periods for low latency playback */
#define PLAYBACK_SHORT_PERIOD_COUNT 1

/* number of periods for capture */
#define CAPTURE_PERIOD_COUNT 2
/* minimum sleep time in out_write() when write threshold is not reached */
#define MIN_WRITE_SLEEP_US 5000

#define RESAMPLER_BUFFER_FRAMES (SHORT_PERIOD_SIZE * 2)
#define RESAMPLER_BUFFER_SIZE (4 * RESAMPLER_BUFFER_FRAMES)

#define DEFAULT_OUT_SAMPLING_RATE 48000

/* sampling rate when using MM full power port */
#define MM_FULL_POWER_SAMPLING_RATE 48000

/* conversions from dB to ABE and codec gains */
#define DB_TO_ABE_GAIN(x) ((x) + MIXER_ABE_GAIN_0DB)
#define DB_TO_CAPTURE_PREAMPLIFIER_VOLUME(x) (((x) + 6) / 6)
#define DB_TO_CAPTURE_VOLUME(x) (((x) - 6) / 6)
#define DB_TO_HEADSET_VOLUME(x) (((x) + 30) / 2)
#define DB_TO_SPEAKER_VOLUME(x) (((x) + 52) / 2)
#define DB_TO_EARPIECE_VOLUME(x) (((x) + 24) / 2)

/* conversions from codec and ABE gains to dB */
#define DB_FROM_SPEAKER_VOLUME(x) ((x) * 2 - 52)

/* use-case specific mic volumes, all in dB */
#define CAPTURE_MAIN_MIC_VOLUME 16
#define CAPTURE_SUB_MIC_VOLUME 18
#define CAPTURE_HEADSET_MIC_VOLUME 12

#define VOICE_RECOGNITION_MAIN_MIC_VOLUME 5
#define VOICE_RECOGNITION_SUB_MIC_VOLUME 18
#define VOICE_RECOGNITION_HEADSET_MIC_VOLUME 14

#define CAMCORDER_MAIN_MIC_VOLUME 13
#define CAMCORDER_SUB_MIC_VOLUME 10
#define CAMCORDER_HEADSET_MIC_VOLUME 12

#define VOIP_MAIN_MIC_VOLUME 13
#define VOIP_SUB_MIC_VOLUME 20
#define VOIP_HEADSET_MIC_VOLUME 12

#define VOICE_CALL_MAIN_MIC_VOLUME 0
#define VOICE_CALL_SUB_MIC_VOLUME -4
#define VOICE_CALL_HEADSET_MIC_VOLUME 8

/* use-case specific output volumes */
#define NORMAL_SPEAKER_VOLUME 2
#define NORMAL_HEADSET_VOLUME -12
#define NORMAL_HEADPHONE_VOLUME -6
#define NORMAL_EARPIECE_VOLUME -2

#define VOICE_CALL_SPEAKER_VOLUME 6
#define VOICE_CALL_HEADSET_VOLUME 0
#define VOICE_CALL_EARPIECE_VOLUME 6

#define VOIP_SPEAKER_VOLUME 7
#define VOIP_HEADSET_VOLUME -6
#define VOIP_EARPIECE_VOLUME 6

#define HEADPHONE_VOLUME_TTY -2
#define RINGTONE_HEADSET_VOLUME_OFFSET -14

#endif // AUDIO_HW_H
