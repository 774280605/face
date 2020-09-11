/*
 *  @file: fsl0_common_struct.h
 *  @brief: fsl0 struct file 
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

#include "fsl0_macro.h"
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

/**
 * SDK handle
 */
typedef void*       MGVL0_HANDLE_T;
typedef void*		MGVL0_MODEL_T;

#define MGVL0_INVALID_FRAME_ID 	(uint64_t)0xFFFFFFFFFFFFFFFF

/**
 * SDK api return code
 */
typedef int         MGVL0_RET_CODE_E;

/**
 * return error code
 */
#define MGVL0_RET_ALG_NO_FACE                   (-1101) // Algorithm didn't detect the face.
#define MGVL0_RET_ALG_REPLY_UNKNOWN             (-1102) // Unknown error occurred inside the algorithm.
#define MGVL0_RET_ALG_LOW_QUALITY               (-1103) // poor quality of image

#define MGVL0_RET_MODEL_TOO_MUCH				 (-55)  // Too many models were loaded.
#define MGVL0_RET_MODEL_HANDLE_ERROR             (-54)  // Model handle error
#define MGVL0_RET_MODEL_TYPE_MISSING             (-53)  // The necessary model was missing.
#define MGVL0_RET_MODEL_VERSION_NOSUPPORT        (-52)  // unsupported version of model
#define MGVL0_RET_MODEL_TYPE_NOSUPPORT           (-51)  // unsupported type of model
#define MGVL0_RET_MODEL_FILE_ERROR               (-50)  // model file error

#define MGVL0_RET_INTERFACE_DISCARD				 (-41)  // discarded interface			
#define MGVL0_RET_INTERFACE_NOSUPPORT            (-40)  // unsupported interface

#define MGVL0_RET_AGAIN                           (-11) // need to try again
#define MGVL0_RET_NO_BUFF                         (-10) // full cache

#define MGVL0_RET_MODEL_PATH_NULL                  (-5) // the model path was NULL.
#define MGVL0_RET_PARAM_ERROR                      (-4) // param error
#define MGVL0_RET_LICENSE_CHECK_ERROR              (-3) // chip was checked failure
#define MGVL0_RET_LICENSE_TIMEOUT                  (-2) // tried timeout
#define MGVL0_RET_ERROR                            (-1) // general error
#define MGVL0_RET_OK                                (0) // success

/**
 * decice type
 */
typedef enum MGVL0_DEVICE_ENUM
{
    MGVL0_DEV_DEFAULT=0,        // use default settings in the model bin file
    MGVL0_DEV_CPU = 1,         
    MGVL0_DEV_GPU=64,
    MGVL0_DEV_HISI_NNIE=128,
    MGVL0_DEV_DSP=192,
    MGVL0_DEV_MAX,
}MGVL0_DEVICE_E;

/**
 * model type
 */
typedef enum MGVL0_MODEL_TYPE_ENUM{
	MGVL0_MODEL_TYPE_DETECT = 0x00,
	MGVL0_MODEL_TYPE_FEATURE,
	MGVL0_MODEL_TYPE_ATTRIBUTE,
	MGVL0_MODEL_TYPE_LIVENESS,
	MGVL0_MODEL_TYPE_MAX,
}MGVL0_MODEL_TYPE_E;

/**
 * model config
 */
typedef struct MGVL0_MODEL_CFG_ST
{
	MGVL0_MODEL_TYPE_E type;
    MGVL0_DEVICE_E device;      // not used now, reserverd
	union
    {
		int reserved[128];
	};
}MGVL0_MODEL_CFG_S;

/**
 * model ver
 */
typedef struct MGVL0_MODEL_VER_ST{

	MGVL0_MODEL_TYPE_E type;
	const char	*version;
}MGVL0_MODEL_VER_S;

/**
 * supported image type
 */
typedef enum MGVL0_IMAGE_TYPE_ENUM 
{
    MGVL0_IMAGE_NV21 = 0,
    MGVL0_IMAGE_GRAY,
    MGVL0_IMAGE_BGR,
} MGVL0_IMAGE_TYPE_E;

/**
 * supported image rotate degree
 */
typedef enum MGVL0_IMAGE_ROTATION_ENUM 
{
    MGVL0_ROTATION_DEG0   = 0,
    MGVL0_ROTATION_DEG90  = 90,
    MGVL0_ROTATION_DEG180 = 180,
    MGVL0_ROTATION_DEG270 = 270,
	MGVL0_ROTATION_MAX,
} MGVL0_IMAGE_ROTATION_E;

/**
 * frame addr
 */
typedef struct MGVL0_IMAGE_ADDR_ST
{
    uint32_t  stride[3];                    // Y U V stride
    void*     phy_addr[3];
    void*     vir_addr[3];                  // pointer of Y U V
} MGVL0_IMAGE_ADDR_S;

/**
 * frame data source type
 */
typedef enum MGVL0_IMAGE_SOURCE_TYPE_ENUM
{
    MGVL0_IMAGE_SOURCE_ADDR = 0x00,         // pyh + virtual address
    MGVL0_IMAGE_SOURCE_DATA = 0x01,         // only use virtual address 
	MGVL0_IMAGE_SOURCE_MAX,
} MGVL0_IMAGE_SOURCE_TYPE_E;


typedef struct MGVL0_IMAGE_ST{
	MGVL0_IMAGE_SOURCE_TYPE_E       addr_type;      // type of frame address
    union 
    {
        MGVL0_IMAGE_ADDR_S          addr;
        unsigned char*              data;
    };

    int                             width;          // width of image
    int                             height;         // height of image
    MGVL0_IMAGE_ROTATION_E  		rotation;		
	MGVL0_IMAGE_TYPE_E      		format;

	int								reserved[32];
}MGVL0_IMAGE_S;

/**
 *  frame structure from the camera used by SDK to detect
 */
typedef struct MGVL0_FRAME_ST
{
    uint64_t                        frame_id;       // frame id //seq of the frame for input
    uint64_t						frame_pts;		// PTS of the frame//not used now //reserverd
	MGVL0_IMAGE_S					image;

    int                             reserved[20];   // reserverd
} MGVL0_FRAME_S;

/**
 * point info 
 */
typedef struct MGVL0_POINT_ST
{
	int x;
	int y;
}MGVL0_POINT_S;

/**
 * rect info 
 */
typedef struct MGVL0_RECT_ST 
{
    int left;
    int top;
    int right;
    int bottom;
} MGVL0_RECT_S;

/**
* 4 point rect info 
**/
typedef struct MGVL0_4POINT_RECT_ST
{
    int x1;// top left corner
    int y1;
    int x2;// top right corner
    int y2;
    int x3;// right bottom corner
    int y3;
    int x4;// left bottom corner
    int y4;
}MGVL0_4POINT_RECT_S;

/**
 * 3D pose
 */
typedef struct MGVL0_POSE_ST 
{
    float roll;
    float pitch;
    float yaw;
} MGVL0_POSE_S;

/**
 * landmark result
 */
typedef struct MGVL0_LANDMARK_RESULT_ST
{
    char* landmark_data;
    int landmark_length;
} MGVL0_LANDMARK_RESULT_S;

/**
 * feature result
 */
typedef struct MGVL0_FEATURE_RESULT_ST
{
    char* feature_data;
    int feature_length;
} MGVL0_FEATURE_RESULT_S;

/**
 * A face image
 */
typedef struct MGVL0_FACE_IMAGE_ST 
{
    uint64_t track_id;                  // id of long type as an identity of track to mark a face in continuous motion
    uint64_t frame_id;                  // frame id //id corresponding to the face information
    MGVL0_IMAGE_S image;
    MGVL0_RECT_S  rect;                 // face coordinates from detection
    MGVL0_LANDMARK_RESULT_S lmk;        // face landmark(key points)
    MGVL0_POSE_S pose;                  // pose of face
    float blur;                         // blur of face(0~1 //0:clearest，1:most blurred)
    float quality;                      // according to pose/blur, face score(0~1，1:highest, best face)
    int reserved[27];  
} MGVL0_FACE_IMAGE_S;

/**
* Does the face meet the threshold requirement
**/
typedef enum MGVL0_FACE_ERROR_CODE_ENUM
{
    MGVL0_FACE_NO_ERROR=0x00,  	            // meet every thresholds
    MGVL0_FACE_ROLL_BAD,  	                // Not meeting the roll requirement
    MGVL0_FACE_YAW_BAD,			   	        // Not meeting the yaw requirement
	MGVL0_FACE_PITCH_BAD,			        // Not meeting the pitch requirement
    MGVL0_FACE_BLUR_BAD,			   	    // Not meeting the blur requirement
    MGVL0_FACE_SIZE_BAD,			   	    // Not meeting the face size requirement
    MGVL0_FACE_BRIGHTNESS_BAD,  	   	    // Not meeting the face brightness requirement
    MGVL0_FACE_LOCATION_BAD                 // face is at the edge of the image
}MGVL0_FACE_ERROR_CODE_E;


