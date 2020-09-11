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
#include "demo_comm.h"
#include "fsl0_interface.h"


#ifdef __cplusplus
extern "C" {
#endif


//Get the current time in milliseconds
unsigned long long get_ms();

//Get the VM rss memory size of the process, KB
int get_vm_rss(void);

//Get the VM memory size of the process, KB
int get_vm_size(void);

//Read file. free (read buf) is required after use
int read_file(char ** read_buf, const char* file_name);

//copy file, Return value: 0 success,  other: error
int copy_file(char *oldpath, char *newpath);

int nv21_image_draw_rect(MGVL0_IMAGE_S *nv21_image,MGVL0_RECT_S rect);

#define time_after(a,b)  ((long)(b) - (long)(a) < 0)

#ifdef __cplusplus
}
#endif

