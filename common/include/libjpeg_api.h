#pragma once
#include "fsl0_interface.h"

#ifdef __cplusplus
    extern "C" {
#endif  //@__cplusplus

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>

#include <stdarg.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/stat.h> 

#include <pthread.h>

#include <assert.h>

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

typedef char                    CHAR;
typedef unsigned char           UCHAR;
typedef unsigned char           BYTE;

typedef unsigned char           UINT8;
typedef unsigned short          UINT16;
typedef unsigned long long      UINT64;
typedef unsigned long int       ULONG;
typedef unsigned int            UINT32;

typedef signed char             INT8;
typedef signed short            INT16;
typedef signed long long        INT64;
typedef signed long int         LONG;

MGVL0_RET_CODE_E sdk_read_jpeg(const char* filename, MGVL0_IMAGE_S *image);
MGVL0_RET_CODE_E sdk_save_nv21_jpeg(char *filename, char *yuv_data, int width, int height);
MGVL0_RET_CODE_E sdk_save_rect_jpg_from_nv21(char * file_path, char *nv21_addr, int nv21_width, int nv21_height, 
                                        MGVL0_RECT_S rect, float rect_extent_ratio);




#ifdef __cplusplus
    }
#endif //@__cplusplus

