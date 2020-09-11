/*
 *  @file: fsl1_feature.h
 *  @brief: face feature
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
 * feature version
 */
typedef struct MGVL0_FEATURE_VER_ST
{
    char* feature_version;                  // for example: 180816v1.2.3, The first two numbers represent major version
                                            // if the first two numbers(1.2) change, features needs to be updated; 
                                            // if the last number change(.3) change, feature does not need to be updated
    float feature_alpha;
    float feature_beta;
    float vmin;
    float vdiff;
    int feature_byte_size;
    int format;
    int reserve[11];
} MGVL0_FEATURE_VER_S;

/**
 * @fn          mgvl0_create_feature_handle
 * @brief       create a algorithm unit to extract face feature
 * @param[in]   model  
 * @param[out]  handle // handle to algorithm processing unit
 * @return      return 0 if everything is fine. Return negative when any error happens.
 */
MGVL0_API MGVL0_RET_CODE_E mgvl0_create_feature_handle(MGVL0_MODEL_T model, MGVL0_HANDLE_T *handle);

/**
 * @fn          mgvl0_destroy_feature_handle
 * @brief       destroys the algorithm handle
 * @param[in]   handle // handle to algorithm processing unit
 * @return      return 0 if everything is fine. Return negative when any error happens.
 */
MGVL0_API MGVL0_RET_CODE_E mgvl0_destroy_feature_handle(MGVL0_HANDLE_T handle);

/**
 * @fn          mgvl0_extract_feature
 * @brief       extract face feature
 * @param[in]   handle          // handle to algorithm processing unit
                image_array     // face images, requiring face rectangle
                images_count    // face image count
 * @param[out]  result return the result of the algorithm processing //call mgvl0_delete(result) to be released
 * @return      return 0 if everything is fine. Return negative when any error happens.
 */
MGVL0_API MGVL0_RET_CODE_E mgvl0_extract_feature(MGVL0_HANDLE_T handle, MGVL0_FACE_IMAGE_S *image_array, int images_count, MGVL0_FEATURE_RESULT_S **result);

/**
 * @fn          mgvl0_get_feature_version
 * @brief       get the version of the feature model
 * @param[in]   handle  // handle to algorithm processing unit
 * @param[out]  version // version string 
 * @return      return 0 if everything is fine. Return negative when any error happens.
 */
MGVL0_API MGVL0_RET_CODE_E mgvl0_get_feature_version(MGVL0_HANDLE_T handle, MGVL0_FEATURE_VER_S *version);

#ifdef __cplusplus
}
#endif