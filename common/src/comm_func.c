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
#include <fcntl.h>
#include <sys/prctl.h>
#include <stdbool.h>
#include <stdint.h>
#include "comm_func.h"

//Get the current time in milliseconds
unsigned long long get_ms()
{
    struct timeval time_v = { 0 };
    gettimeofday(&time_v, NULL);
    return (unsigned long long)time_v.tv_sec * 1000 + time_v.tv_usec / 1000;
}

int get_vm_rss(void)
{
    FILE *fp = NULL;
    char buf[256] = {};
    int vm_rss = 0;

    fp = fopen("/proc/self/status", "r");
    if(fp)
    {
        while(fgets(buf, sizeof(buf)-1, fp))
        {
            if(strstr(buf, "VmRSS:"))
            {
                sscanf(buf, "VmRSS: %d kB", &vm_rss);
                break;
            }
        }
        fclose(fp);
        fp = NULL;
    }
    return vm_rss;
}

int get_vm_size(void)
{
    FILE *fp = NULL;
    char buf[256] = {};
    int vm_size = 0;

    fp = fopen("/proc/self/status", "r");
    if(fp)
    {
        while(fgets(buf, sizeof(buf)-1, fp))
        {
            if(strstr(buf, "VmSize:"))
            {
                sscanf(buf, "VmSize: %d kB", &vm_size);
                break;
            }
        }
        fclose(fp);
        fp = NULL;
    }
    return vm_size;
}



int read_file(char ** read_buf, const char* file_name)
{
	FILE * pFile = NULL;
	long lSize;
	char * buffer = NULL;
	size_t result;
    int ret = -1;

	pFile = fopen (file_name , "rb" );
	if (pFile == NULL)
	{
		DEBUG("File error:%s !!!\n", file_name);
		ret = -1;
        goto EXIT;
	}

	/* obtain file size: */
	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);

	/* allocate memory to contain the whole file: */
	buffer = (char*) malloc (sizeof(char) * lSize);
	if (buffer == NULL)
	{
		DEBUG("Memory error !!!\n");
		ret = -1;
        goto EXIT;
	}

	/* copy the file into the buffer: */
	result = fread (buffer, 1, lSize, pFile);
	if (result != lSize)
	{
		DEBUG("Reading error !!!\n");
		ret = -1;
        goto EXIT;
	}

	/* the whole file is now loaded in the memory buffer. */

	/* terminate */
	*read_buf = buffer;
    ret = 0;

EXIT:
    if(pFile)
    {
        fclose(pFile);
    }
    if(ret)
    {
        if(buffer)
        {
            free(buffer);
        }
    }

	return ret;
}

int copy_file(char *oldpath, char *newpath)
{
    char buf[4096];
    FILE *in, *out;
    int nread;

    if( (in = fopen(oldpath, "rb" ) ) == NULL )
    {
        perror(oldpath);
        return -errno;
    }

    if( (out = fopen( newpath,  "wb")) == NULL )
    {
        fclose(in);
        perror(newpath);
        return -errno;
    }

    while(( nread = fread(buf, 1, sizeof(buf), in) ) > 0)
    {
        fwrite(buf,  1, nread, out);
    }

    fclose(in);
    fclose(out);
    return 0;    
}

int nv21_image_draw_rect(MGVL0_IMAGE_S *nv21_image, MGVL0_RECT_S rect)
{
    int ret = DEMO_FAIL;
    int box_x = 0;
    int box_y = 0;
    int box_w = 0;
    int box_h = 0;
    int i = 0;
    char *src_y = NULL;
    
    if(!nv21_image)
    {
        DEBUG("input param error!!!\n");
        goto EXIT;    
    }
    box_x = rect.left;
    box_y = rect.top;
    box_w = rect.right - rect.left;
    box_h = rect.bottom - rect.top;

    src_y = (char *)(nv21_image->data + box_y * nv21_image->width + box_x);

    memset(src_y,255,box_w);
    for(i = 0;i < box_h;i++)
    {
        src_y[0] = 255;
        src_y[box_w-1] = 255;
        src_y += nv21_image->width;
    }
    memset(src_y,255,box_w);
    ret = DEMO_OK;
    
EXIT:
    return ret;
}

