#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>
#include <dirent.h>
#include <assert.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/prctl.h>




#ifdef __cplusplus
extern "C" {
#endif

#define ALIGN_UP(x, a)           ( ( ((x) + ((a) - 1) ) / a ) * a )
#define ALIGN_DOWN(x, a)         ( ( (x) / (a)) * (a) )

#define DEMO_OK (0)
#define DEMO_FAIL (-1)
#define DEMO_FILE_LEN (512)
#define DEMO_PATH_LEN (512)
#define MAX_FACE_COUNT (5000)
#define IN
#define OUT
#define DEMO_COMPARE_SCORE 70

#define DEMO_MAX_DATA_NODE (32)
#define DEMO_INVALID_FRAME_ID 	(uint64_t)0xFFFFFFFFFFFFFFFF
#define DEMO_STREAM_MAX_COUNT 	(16)
#define FEAT_TXT_NAME "feat.txt"

//#define OPEN_TRACE
#define Unused __attribute__((unused))

typedef void*     COMM_CAPTURE_HANDLE;
typedef void*     COMM_DETECT_HANDLE;

//#define SAVE_FEATURE_IN_MULTI_GROUP_FOLDER            // Store the feature in multiple folders, which is suitable for the case of large number face in database (such as 300000 faces)
#define MAX_FEATURE_COUNT_IN_ONE_GROUP_FOLDER (5000)    // How many features can be placed in each folder at most
#define FEATURE_DATA_LEN (2400)

#define MAX_FACE_FEATURE_COUNT (128)
#define DEMO_INPUT_WIDTH   (1280)
#define DEMO_INPUT_HEIGHT  (736)
#define DEMO_FRAME_NODE_COUNT (15)
#define DEMO_3559A_FRAME_NODE_COUNT (20)

#define SLASH "/"

typedef enum DEMO_INPUT_FILE_TYPE_ENUM
{
	DEMO_INPUT_FILE_YUV = 0x00,
	DEMO_INPUT_FILE_H264,
	DEMO_INPUT_FILE_RTSP,

}DEMO_INPUT_FILE_TYPE_E;

typedef enum DEMO_SEARCH_TYPE_ENUM
{
    MGVIL_SEARCH = 0,
    COMPARE_FEATURE_MXN,
    MGVIL_SEARCH_AND_COMPARE_FEATURE_MXN,
}DEMO_SEARCH_TYPE_E;

typedef enum DEMO_FEATURE_SAVE_TYPE_ENMU
{
    SAVE_TO_FILE = 0,  //Save the feature results to a file
    SAVE_TO_MEM,       //Save the feature results to memory
}DEMO_FEATURE_SAVE_TYPE_E;

typedef enum DEMO_DBG_LEVEL_ENUM
{
    DEMO_DBG_LEVEL_QUIET = 0,
    DEMO_DBG_LEVEL_ERR = 1,
    DEMO_DBG_LEVEL_WARN,
    DEMO_DBG_LEVEL_TRACE,
    DEMO_DBG_LEVEL_DETAIL,
    DEMO_DBG_LEVEL_FORCE,
}DEMO_DBG_LEVEL_E;

typedef struct COMM_FACE_INFO_ST
{
    char  jpg_path[DEMO_FILE_LEN];
    char  origin_jpg_name[DEMO_FILE_LEN];
    int   feature_size;
    char* feature;
}COMM_FACE_INFO_S;

typedef struct COMM_FACE_GROUP_ST
{
    int face_count;
    COMM_FACE_INFO_S face[MAX_FACE_COUNT];
}COMM_FACE_GROUP_S;

typedef struct COMM_RAW_IMAGE_ST
{
	int  	format;
	int  	height;
	int  	width;
	int  	rotation;
	int	size;

	int	stride[3];
	char*   data[3];
	char*	addr; 

}COMM_RAW_IMAGE_S;

typedef struct COMMON_NV21_IMAGE_ST
{
    unsigned char *data;
    int width;
    int height;
}COMMON_NV21_IMAGE_S;

#define DEMO_MAX_WIDTH 4096
#define DEMO_MAX_HEIGHT 4096
#define DEMO_MIN_WIDTH  50
#define DEMO_MIN_HEIGHT 50



#ifdef OPEN_TRACE

#define DEBUG_TRACE(fmt,...) \
do\
{\
    printf("%s(%d): " fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__);fflush(stdout); \
}\
while(0)

#else

#define DEBUG_TRACE(fmt,...) \
if(0)\
{\
    printf("%s(%d): " fmt,__FUNCTION__, __LINE__, ##__VA_ARGS__);fflush(stdout); \
}


#endif


#define DEBUG(fmt, ...) \
do\
{\
    printf("%s(%d): " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);fflush(stdout); \
}\
while(0)


#define CHECK_RET(express,name) \
do\
{\
    int ret = -1;\
    ret = exporess;\
    if(ret)\
    {\
        printf("%s(%d): %s failed: ret=%x !!!\n ",__FUNCTION__, __LINE__,name, ret);fflush(stdout); \
    }\
}\
while(0)

#ifdef __cplusplus
}
#endif

