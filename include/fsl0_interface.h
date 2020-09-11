/*
 *  @file: fsl0_interface.h
 *  @brief: fsl0 interface 
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
 * sdk parameter
 */
typedef struct MGVL0_SDK_PARAM_ST
{
    int  reserved[16];                      // reserved
}MGVL0_SDK_PARAM_S;

/**
 * @fn          mgvl0_init_sdk
 * @brief       initialize SDK
 * @param[in]   param //initialize param
 * @param[out]  null
 * @return      MGVL1_RET_OK:success,MGVL1_RET_LICENSE_TIMEOUT:trail timeout,MGVL1_RET_LICENSE_CHECK_ERROR:chip verification failed,MGVL1_RET_PARAM_ERROR:paramerror
 */
MGVL0_API MGVL0_RET_CODE_E mgvl0_init_sdk(MGVL0_SDK_PARAM_S *param);

/**
 * @fn          mgvl0_deinit_sdk
 * @brief       deinitialize SDK 
 * @param[in]   
 * @param[out]  null
 * @return      0 success, -1 fail
 * @note        user need  destory handles first
 */
MGVL0_API MGVL0_RET_CODE_E mgvl0_deinit_sdk(void);

/**
 * @fn          mgvl0_add_model
 * @brief       load model//It can be called repeatedly to load different models.
 * @param[in]   file //model file   cfg //type of model
 * @param[out]  model 返回关联模型的句柄
 * @return      return 0 if everything is fine. Return negative when any error happens.
 */
MGVL0_API MGVL0_RET_CODE_E mgvl0_add_model(const char* file, MGVL0_MODEL_CFG_S *cfg, MGVL0_MODEL_T *model);

/**
 * @fn          mgvl0_add_model_from_mem
 * @brief       加载一个model
 * @param[in]   model_buf //model memory   cfg //type of model
 * @param[out]  model //return the handel to association model
 * @return      return 0 if everything is fine. Return negative when any error happens.
 */
MGVL0_API MGVL0_RET_CODE_E mgvl0_add_model_from_mem(const char* model_buf, MGVL0_MODEL_CFG_S *cfg, MGVL0_MODEL_T *model);

/**
 * @fn          mgvl0_del_model
 * @brief       delete model
 * @param[in]   model handle returned after adding successfully
 * @return      return 0 if everything is fine. Return negative when any error happens.
 */
MGVL0_API MGVL0_RET_CODE_E mgvl0_del_model(MGVL0_MODEL_T model);

/**
 * @fn          mgvl0_get_model_version
 * @brief       get model version information
 * @param[in]   model handle returned after adding successfully
 * @param[out]  version //versions corresponding to models
                count //return number of loaded models
 * @return      return 0 if everything is fine. Return negative when any error happens.
 */
MGVL0_API MGVL0_RET_CODE_E mgvl0_get_model_version(MGVL0_MODEL_T model, const MGVL0_MODEL_VER_S** version, int *count);

/**
 * @fn          mgvl0_set_log_level
 * @brief       set print level of log
 * @param[in]   level //print level 0 default, 1 debug
 * @param[out]  null
 * @return      0 success, -1 fail
 */
MGVL0_API MGVL0_RET_CODE_E mgvl0_set_log_level(int level);

/**
 * @fn          mgvl0_get_version
 * @brief       get version of SDK 
 * @param[in]   null
 * @param[out]  version //pointer of returning version
 * @return      0 success, -1 fail
 */
MGVL0_API MGVL0_RET_CODE_E mgvl0_get_version(const char **version);

/**
 * @fn          mgvl0_delete
 * @brief       release data memory allocated in mgvl0
 * @param[in]   p //pointer that should be released
 * @param[out]  null
 * @return      0 success, -1 fail
 */
MGVL0_API MGVL0_RET_CODE_E mgvl0_delete(void *p);

#ifdef __cplusplus
}
#endif
