/*
 *  @file    : fsl0_compare.h
 *  @brief   : feature compare 
 *  @version : 0.0.1
 *  @author  : 
 *  @date    : 2019-5-20 15:50:50
 */
 
/******************************************************************************
@note
    Copyright 2019, Megvii Corporation, Limited
                        ALL RIGHTS RESERVED
******************************************************************************/

#pragma once

#include "fsl0_common_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * One face is compared with faces in group to get the result sorted by score.
 * Print the top top_k scores.
 */
typedef struct MGVL0_COMPARE_SCORE_ST {
    size_t              top_k;              // count of top scores
    float*              scores;             // scores array, contain top_k items
    int *               face_id;            // face id array, contain top_k items
} MGVL0_COMPARE_SCORE_S;

/**
 * Multiple faces are compared with faces in group to get the result.
 */
typedef struct MGVL0_FEATURE_COMPARE_RESULT_ST 
{
    MGVL0_COMPARE_SCORE_S   *score_list;    // list of comparison results
    int                     score_list_size;// number of results
} MGVL0_FEATURE_COMPARE_RESULT_S;

/**
 * @fn          mgvl0_create_compare_handle
 * @brief       create a algorithm unit to compare feature
 * @param[in]   model         
 * @param[out]  handle      // handle to algorithm processing unit
 * @return[out] 0 success,  other:fail
 */
MGVL0_API MGVL0_RET_CODE_E mgvl0_create_compare_handle(MGVL0_MODEL_T model, MGVL0_HANDLE_T *handle);

/**
 * @fn          mgvl0_destroy_compare_handle
 * @brief       destroy algorithm unit
 * @param[in]  handle       // handle to algorithm processing unit   
 * @return[out] 0 success,  other:fail
 */
MGVL0_API MGVL0_RET_CODE_E mgvl0_destroy_compare_handle(MGVL0_HANDLE_T handle);

/**
 * @fn          mgvl0_compare_feature
 * @brief       face comparison 1:1
 * @param[in]   handle                    // handle to algorithm processing unit
                MGVL0_FEATURE_S featureA  // feature of face 1
                MGVL0_FEATURE_S featureB  // feature of face 2
                float *result             // score of comparing two faces(0-100) //the higher the score is, the more similar the faces are.
 * @return[out] 0 success,  other:fail
*/
MGVL0_API MGVL0_RET_CODE_E mgvl0_compare_feature(MGVL0_HANDLE_T handle, const MGVL0_FEATURE_RESULT_S *featureA, const MGVL0_FEATURE_RESULT_S *featureB, float* result); 

/**
* @fn          mgvl0_compare_feature_mxn
* @brief       face comparison M:N
* @param[in]   handle //handle to algorithm processing unit
               features_array1  //feature array1
               array1_count     //number of elements in features_array1
               features_array2  //feature array2
               array2_count     //number of elements in features_array2
               top_k            //the top k scores
* @param[out]  compare_result   //return the result of the algorithm processing //call mgvl1_delete to be released
* @return      0 success, -1 fail
*/
MGVL0_API MGVL0_RET_CODE_E mgvl0_compare_feature_mxn(MGVL0_HANDLE_T handle, 
                                            MGVL0_FEATURE_RESULT_S *features_array1, int array1_count,
                                            MGVL0_FEATURE_RESULT_S *features_array2, int array2_count,
                                            int top_k,
                                            MGVL0_FEATURE_COMPARE_RESULT_S ** compare_result);

#ifdef __cplusplus
}
#endif
