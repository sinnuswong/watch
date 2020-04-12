/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if !defined(_CLASSIC_WATCH_H)
#define _CLASSIC_WATCH_H

#if !defined(PACKAGE)
#define PACKAGE "org.example.chronographwatch"
#endif

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "chronographwatch"

typedef enum {
	PARTS_TYPE_HANDS_SEC = 0,
	PARTS_TYPE_HANDS_MIN = 1,
	PARTS_TYPE_HANDS_HOUR = 2,
	PARTS_TYPE_HANDS_SEC_SHADOW = 3,
	PARTS_TYPE_HANDS_MIN_SHADOW = 4,
	PARTS_TYPE_HANDS_HOUR_SHADOW = 5,
	PARTS_TYPE_HANDS_STOPWATCH_SEC = 6,
	PARTS_TYPE_HANDS_STOPWATCH_30S = 7,
	PARTS_TYPE_HANDS_STOPWATCH_12H = 8,
	PARTS_TYPE_MAX,
} parts_type_e;

#define PARTS_TYPE_NUM 9

/* Angle */
#define HOUR_ANGLE 30
#define MIN_ANGLE 6
#define SEC_ANGLE 6

/* Layout */
#define BASE_WIDTH 360
#define BASE_HEIGHT 360

#define HANDS_SEC_BG_SIZE 126
#define HANDS_SEC_WIDTH 12
#define HANDS_SEC_HEIGHT 126
#define HANDS_MIN_WIDTH 28
#define HANDS_MIN_HEIGHT 360
#define HANDS_HOUR_WIDTH 28
#define HANDS_HOUR_HEIGHT 360

#define HANDS_SW_SEC_WIDTH 20
#define HANDS_SW_SEC_HEIGHT 360

#define HANDS_SEC_SHADOW_PADDING 3
#define HANDS_MIN_SHADOW_PADDING 6
#define HANDS_HOUR_SHADOW_PADDING 6

#define HANDS_SEC_POSITION_Y 117

#define HANDS_SEC_PIVOT_POSITION_X 93
#define HANDS_SEC_PIVOT_POSITION_Y BASE_HEIGHT / 2

#define HANDS_STOPWATCH_30S_PIVOT_POSITION_X BASE_WIDTH /2
#define HANDS_STOPWATCH_30S_PIVOT_POSITION_Y 93
#define HANDS_STOPWATCH_12H_PIVOT_POSITION_X BASE_WIDTH /2
#define HANDS_STOPWATCH_12H_PIVOT_POSITION_Y 360-93

#endif
