/*
 *  @file: fsl0_detector.h
 *  @brief: fsl0_detect 
 *  @version: 0.0.1
 *  @author:     
 *  @date: 
 */
 
/******************************************************************************
@note
    Copyright 2017, Megvii Corporation, Limited
                        ALL RIGHTS RESERVED
******************************************************************************/

#pragma once

#include "fsl0_common_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * detect configuration
 */
typedef struct MGVL0_FACE_DETECT_CONFIG_ST 
{
    int face_min;                           // minimum pixels for face image capture (0-maximum resolution, ignoring small face)
	float	pose_roll_upper_threshold;      // pose constraint of capturing face (ignoring face with larger angle. range 0-180 degrees)
	float	pose_yaw_upper_threshold;
	float	pose_pitch_upper_threshold;
	float	blurriness_upper_threshold;     // blur constraint of capturing face (ignoring the more fuzzy face. range 0.0-1.0)
	int     brightness_low_threshold;       // Lower limit of face brightness, range (0-255), recommended value 70, set to 0 to indicate unlimited
    int     brightness_upper_threshold;     // Upper limit of face brightness, range (0-255), recommended value 210, set to 0 to indicate unlimited
    int     brightness_deviation_threshold; // Face brightness standard deviation threshold, Range (0-255), recommended value 60, set to 0 means unlimited
                                            // such as face with sunglasses, the luminance standard deviation will be larger, so the face quality can be considered as poor. 
    float   face_completeness_threshold;    // face integrity, range (0 ~ 1), the recommended value is 0.90. When set to 0.0, the default value of 0.90 is used inside the SDK
                                            // if face locate at the edge of the image, this value will be low.
    int reserved[23];                       // reserved
} MGVL0_FACE_DETECT_CONFIG_S;

/**
* detect face structure for SDK user 
**/
typedef struct MGVL0_DETECT_FACE_ST
{
	MGVL0_RECT_S rect;                  // face rectangle
    MGVL0_RECT_S extent_rect;           // expendrd face rect, recommended matting coordinates for attribute calculation
    MGVL0_POSE_S pose;					// face pose 
	float blur; 						// blur of face(0~1 //0:clearest，1:most blurred)
    MGVL0_LANDMARK_RESULT_S landmark;   // face landmark(key points)
	MGVL0_FACE_ERROR_CODE_E errcode;    // whether the face meets every threshold limits
	int brightness;                     // face brightness
    int brightness_deviation;           // standard deviation of face brightness
    float face_completeness;            // face integrity. if face locate at the edge of the image, this value will be low.
    int reserved[23];					// reserved
} MGVL0_DETECT_FACE_S;

/**
 * result of detection
 */
typedef struct MGVL0_FACE_DETECT_RESULT_ST 
{
    MGVL0_DETECT_FACE_S  *face_list;    // list of detected faces
    int face_list_size;                 // face count

	uint64_t frame_id;
	
    int reserved[24];					// reserved
} MGVL0_FACE_DETECT_RESULT_S;

/**
 * @fn          mgvl0_create_detect_handle
 * @brief       create a detection algorithm unit
 * @param[in]   model  
 * @param[out]  handle // handle to algorithm processing unit
 * @return      return 0 if everything is fine. Return negative when any error happens.
 */
MGVL0_API MGVL0_RET_CODE_E mgvl0_create_detect_handle(MGVL0_MODEL_T model, MGVL0_HANDLE_T *handle);

/**
 * @fn          mgvl0_destroy_detect_handle
 * @brief       destroys the algorithm handle
 * @param[in]   handle // handle to algorithm processing unit
 * @return      return 0 if everything is fine. Return negative when any error happens.
 */
MGVL0_API MGVL0_RET_CODE_E mgvl0_destroy_detect_handle(MGVL0_HANDLE_T handle);

/**
 * @fn          mgvl0_detect_face
 * @brief       detect face in a photo
 * @param[in]   handle      // handle to algorithm processing unit
                config      // filter parameters. Faces less than parameters will be marked as bad faces (errcode != 0). 
                                set null means no filtering
                frame       // photo
 * @param[out]  result //return the result of the algorithm processing //call mgvl0_delete(result) to be released
 * @return      return 0 if everything is fine. Return negative when any error happens.
 */
MGVL0_API MGVL0_RET_CODE_E mgvl0_detect_face(MGVL0_HANDLE_T handle, MGVL0_FACE_DETECT_CONFIG_S *config, 
                                            MGVL0_FRAME_S *frame, MGVL0_FACE_DETECT_RESULT_S **result);

#ifdef __cplusplus
}
#endif

