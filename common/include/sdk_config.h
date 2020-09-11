#pragma once

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "fsl0_interface.h"

#define MAX_VIDEO_CHANNEL_NUM (4)   // Maximum channels of video stream

#define MAX_VIDEO_WIDTH     (1920)  // Maximum video width and height
#define MAX_VIDEO_HEIGHT    (1080)

#define MAX_PHOTO_WIDTH     (1920)  // Maximum photo width and height
#define MAX_PHOTO_HEIGHT    (1080)

#define FACE_COMMON_WIDTH   (320)   // The width and height of the captured face image, related to the actual scene
#define FACE_COMMON_HEIGHT  (320)


#define DET_MODEL_FILE      "../../models/M_det_x86_v1.2.bin"
#define FEATURE_MODEL_FILE  "../../models/M_feature_x86_cusk_v1.2.bin"

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
