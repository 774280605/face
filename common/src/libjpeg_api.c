/*
 *  @file: fsl1_interface.c
 *  @brief:
 *  @version: 0.0
 *  @author:
 *  @date: 2017/12/22
 */
/******************************************************************************
@note
    Copyright 2017, Megvii Corporation, Limited
                            ALL RIGHTS RESERVED
******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "fsl0_interface.h"
#include "demo_comm.h"
#include "jpeglib.h"
#include "turbojpeg.h"

static int sdk_image_convert_420_to_nv21(unsigned char *data, int width , int height, MGVL0_IMAGE_S *dst_image)
{
    int ret = MGVL0_RET_ERROR;
    int src_y_len  = height * width;
    int row = 0, column=0;
    dst_image->addr_type = MGVL0_IMAGE_SOURCE_DATA;
    dst_image->height = height / 2 * 2;
    dst_image->width = width / 2 * 2;
    dst_image->rotation = MGVL0_ROTATION_DEG0;
    dst_image->format = MGVL0_IMAGE_NV21;
    dst_image->data = (unsigned char *)malloc(dst_image->height * dst_image->width * 3 / 2);
    if(!dst_image->data)
    {
        DEBUG("malloc dst_image->data error !!!\n");
        return MGVL0_RET_ERROR;
    }

    int dst_y_len  = dst_image->height * dst_image->width;
    unsigned char *src_y = data;
    unsigned char *src_u = src_y + src_y_len;
    unsigned char *src_v = src_u + (src_y_len >> 2);

    unsigned char *dst_y  = dst_image->data;
    unsigned char *dst_uv = dst_y + dst_y_len;

    for(row = 0; row < dst_image->height; row++)
    {
        memcpy(&dst_y[row * dst_image->width], &src_y[row * width], dst_image->width);
    }

    for(row = 0; row < dst_image->height; row += 2)
    {
        for(column = 0; column < dst_image->width; column += 2)
        {
            *dst_uv++ = *src_v++;
            *dst_uv++ = *src_u++;
        }
    }

#if 0
    printf("src: width[%d] height[%d] dst: width[%d] height[%d]\n", \
           nv21_height, src_image->height, dst_image->width, dst_image->height);
#endif

    ret = MGVL0_RET_OK;

    return ret;
}


static int sdk_image_convert_444_to_nv21(unsigned char *data, int width , int height, MGVL0_IMAGE_S *dst_image)
{
    int ret = MGVL0_RET_ERROR;
    int src_y_len  = height * width;
    int row = 0, column=0;
    dst_image->addr_type = MGVL0_IMAGE_SOURCE_DATA;
    dst_image->height = height / 2 * 2;
    dst_image->width = width / 2 * 2;
    dst_image->rotation = MGVL0_ROTATION_DEG0;
    dst_image->format = MGVL0_IMAGE_NV21;
    dst_image->data = (unsigned char *)malloc(dst_image->height * dst_image->width * 3 / 2);
    if(!dst_image->data)
    {
        DEBUG("malloc dst_image->data error !!!\n");
        return MGVL0_RET_ERROR;
    }

    int dst_y_len  = dst_image->height * dst_image->width;
    unsigned char *src_y = data;
    unsigned char *src_u = src_y + src_y_len;
    unsigned char *src_v = src_u + src_y_len;

    unsigned char *dst_y  = dst_image->data;
    unsigned char *dst_uv = dst_y + dst_y_len;

    for(row = 0; row < dst_image->height; row++)
    {
        memcpy(&dst_y[row * dst_image->width], &src_y[row * width], dst_image->width);
    }

    for(row = 0; row < dst_image->height; row+=2)
    {
        unsigned char *p_line_v = &src_v[row*width];
        unsigned char *p_line_u = &src_u[row*width];        
        for(column = 0; column < dst_image->width; column+=2)
        {
            *dst_uv++ = *p_line_v;
            p_line_v += 2;
            *dst_uv++ = *p_line_u;
            p_line_u += 2;
        }
    }

#if 0
    printf("src: width[%d] height[%d] dst: width[%d] height[%d]\n", \
           nv21_height, src_image->height, dst_image->width, dst_image->height);
#endif

    ret = MGVL0_RET_OK;

    return ret;
}

static int sdk_image_convert_422_to_nv21(unsigned char *data, int width , int height, MGVL0_IMAGE_S *dst_image)
{
    int ret = MGVL0_RET_ERROR;
    int src_y_len  = height * width;
    int row = 0, column=0;
    dst_image->addr_type = MGVL0_IMAGE_SOURCE_DATA;
    dst_image->height = height / 2 * 2;
    dst_image->width = width / 2 * 2;
    dst_image->rotation = MGVL0_ROTATION_DEG0;
    dst_image->format = MGVL0_IMAGE_NV21;
    dst_image->data = (unsigned char *)malloc(dst_image->height * dst_image->width * 3 / 2);
    if(!dst_image->data)
    {
        DEBUG("malloc dst_image->data error !!!\n");
        return MGVL0_RET_ERROR;
    }

    int dst_y_len  = dst_image->height * dst_image->width;
    unsigned char *src_y = data;
    unsigned char *src_u = src_y + src_y_len;
    unsigned char *src_v = src_u + (src_y_len >> 2);

    unsigned char *dst_y  = dst_image->data;
    unsigned char *dst_uv = dst_y + dst_y_len;
    
    for(row = 0; row < dst_image->height; row++)
    {
        memcpy(&dst_y[row*dst_image->width], &src_y[row*width], dst_image->width);
    }
    
    for(row = 0; row < dst_image->height; row+=2)
    {
        unsigned char *p_line_v = &src_v[row*width/2];
        unsigned char *p_line_u = &src_u[row*width/2];
        for(column = 0; column < dst_image->width; column+=2)
        {
            *dst_uv++ = *p_line_v++;
            *dst_uv++ = *p_line_u++;
        }
    }

#if 0
    printf("src: width[%d] height[%d] dst: width[%d] height[%d]\n", \
           nv21_height, src_image->height, dst_image->width, dst_image->height);
#endif

    ret = MGVL0_RET_OK;

    return ret;
}


MGVL0_RET_CODE_E sdk_convert_jpg_to_nv21(char *jpg_data, int jpg_size, MGVL0_IMAGE_S *nv21_frame)
{
    int ret = MGVL0_RET_ERROR;
    unsigned char *yuv_data = NULL;
    int width = 0, height = 0;
    int subsample = 0, colorspace = 0;
    unsigned long need_size = 0;
    tjhandle handle_decompress = NULL;

    if(!jpg_data || jpg_size <= 0 || !nv21_frame)
    {
        DEBUG("input param error!!!\n");
        ret = MGVL0_RET_ERROR;
        goto EXIT;
    }

    handle_decompress = tjInitDecompress();
    if(!handle_decompress)
    {
        DEBUG("tjInitDecompress error !!!\n");
        goto EXIT;
    }

    ret = tjDecompressHeader3(handle_decompress, (unsigned char *)jpg_data, jpg_size,
                              &width, &height, &subsample, &colorspace);
    if(ret)
    {
        DEBUG("tjDecompressHeader3 error: ret=%d[%s] !!!\n", ret, tjGetErrorStr());
        goto EXIT;
    }

#if 0
    printf("decode:width----------->%d\n", width);
    printf("decode:height---------->%d\n", height);
    printf("decode:subsample------->%d\n", subsample);
    printf("decode:colorspace------>%d\n", colorspace);
#endif
    need_size = tjBufSizeYUV(width, height, subsample);
    yuv_data = (unsigned char *)malloc(need_size * sizeof(char));
    if(!yuv_data)
    {
        DEBUG("malloc need buff error:%lu!!!\n", need_size);
        goto EXIT;
    }

    //@  pad must use 1
    ret = tjDecompressToYUV2(handle_decompress, (unsigned char *)jpg_data, jpg_size, yuv_data, width, 1, height, 0);
    if (ret < 0)
    {
        DEBUG("tjDecompressToYUV2 error: ret=%d[%s] !!!\n", ret, tjGetErrorStr());
        goto EXIT;
    }

    if(subsample == TJSAMP_420)
    {
        width  += width%2;
        height += height%2;
        ret = sdk_image_convert_420_to_nv21(yuv_data, width, height, nv21_frame);
        if(ret)
        {
            DEBUG("sdk_image_convert_420_to_nv21 !!!\n");
            goto EXIT;
        }
    }
    else if(subsample == TJSAMP_444)
    {
        ret = sdk_image_convert_444_to_nv21(yuv_data, width, height, nv21_frame);
        if(ret)
        {
            DEBUG("sdk_image_convert_444_to_nv21 !!!\n");
            goto EXIT;
        }
    }    
    else if(subsample == TJSAMP_422)
    {
        width  += width%2;
        ret = sdk_image_convert_422_to_nv21(yuv_data, width, height, nv21_frame);
        if(ret)
        {
            DEBUG("sdk_image_convert_422_to_nv21 !!!\n");
            goto EXIT;
        }
    }
    else
    {
        ret = MGVL0_RET_ERROR;
        DEBUG("decode YUV not support subsample%d !!!\n", subsample);
        goto EXIT;
    }

    ret = 0;

EXIT:

    if(yuv_data)
    {
        free(yuv_data);
    }

    if(handle_decompress)
    {
        tjDestroy(handle_decompress);
    }
    return ret;
}

MGVL0_RET_CODE_E sdk_read_jpeg(const char* filename, MGVL0_IMAGE_S *image)
{
    FILE *fp = NULL;
    char *jpg_data = NULL;
    int  jpg_size = 0;
    int  ret = 0;

    fp = fopen(filename, "rb+");
    if(fp)
    {
        fseek(fp, 0, SEEK_END);
        jpg_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        jpg_data = (char*)malloc(jpg_size);

        fread(jpg_data, 1, jpg_size, fp);
        fclose(fp);
        ret = sdk_convert_jpg_to_nv21(jpg_data, jpg_size, image);
        free(jpg_data);
        if(ret)
        {
            DEBUG("sdk_convert_jpg_to_nv21 fail,%s\n", filename);
        }
    }
    else
    {
        DEBUG("fopen fail\n");
    }
    return ret;
}


MGVL0_RET_CODE_E sdk_save_nv21_jpeg(char *filename, char *yuv_data, int width, int height)
{
    int ret = MGVL0_RET_ERROR;

    tjhandle handle_compress = NULL;
    int subsample = 0;
    int flag = 0, pad = 0;
    int quality = 50;
    int i = 0;
    unsigned char *yuv_buf = NULL;
    unsigned char *p_src_uv = NULL;
    unsigned char *p_dst_u = NULL;
    unsigned char *p_dst_v = NULL;

    unsigned char *jpeg_buff = NULL;
    unsigned long jpeg_size = 0;

    int image_width = 0;
    int image_height = 0;
    FILE *fp = NULL;

    if(!filename || !yuv_data)
    {
        printf("%d input param is NULL !!!\n", __LINE__);
        return MGVL0_RET_ERROR;
    }

    handle_compress = tjInitCompress();
    if(!handle_compress)
    {
        printf("tjInitCompress error !!!\n");
        ret = MGVL0_RET_ERROR;
        goto EXIT;
    }

    pad = 1;
    flag |= 0;
    subsample = TJSAMP_420;
    image_width = width;
    image_height = height;

    yuv_buf = (unsigned char *)malloc(image_width * image_height * 3 / 2);
    if(yuv_buf == NULL)
    {
        ret = -1;
        goto EXIT;
    }
    //nv21 -> yuv420
    memcpy(yuv_buf, yuv_data, image_width * image_height); //y
    p_src_uv = (unsigned char *)yuv_data + image_width * image_height;
    p_dst_u = yuv_buf + image_width * image_height;
    p_dst_v = p_dst_u + image_width * image_height / 4;
    for(i = 0; i < image_width * image_height / 4; i++)
    {
        *p_dst_v++ = *p_src_uv++;
        *p_dst_u++ = *p_src_uv++;
    }

    ret = tjCompressFromYUV(handle_compress, yuv_buf, image_width,
                            pad, image_height, subsample, &jpeg_buff, &jpeg_size,
                            quality, flag);
    if (ret < 0)
    {
        printf("tjCompressFromYUV error, err msg=[%s]!!!\n", tjGetErrorStr());
        ret = MGVL0_RET_ERROR;
        goto EXIT;
    }

    fp = fopen(filename, "wb");
    if(fp)

    {
        fwrite(jpeg_buff, jpeg_size, 1, fp);
        fclose(fp);
        ret = MGVL0_RET_OK;
    }

EXIT:
    if(jpeg_buff)
        tjFree(jpeg_buff);

    if(handle_compress)
        tjDestroy(handle_compress);

    if(yuv_buf)
        free(yuv_buf);

    return ret;
}

MGVL0_RET_CODE_E sdk_save_rect_jpg_from_nv21(char * file_path, char *nv21_addr, int nv21_width, int nv21_height, MGVL0_RECT_S rect, float rect_extent_ratio)
{
    int ret = MGVL0_RET_ERROR;
    int32_t x_offset = 0, y_offset = 0;
    int32_t row = 0, i = 0;
    int32_t width = 0, height = 0;
    char *src_y = NULL, *dst_y = NULL;
    char *src_uv = NULL, *dst_uv = NULL;
    float face_rect_scale = rect_extent_ratio;
    MGVL0_RECT_S real_rect = {0};
    char *dst_image_data = NULL;
    
    if(!file_path || !nv21_addr)
    {
        DEBUG("input param is NULL[%p|%p] !!!\n", file_path, nv21_addr);
        ret = -1;
        goto EXIT;
    }

    if(rect.top<0 || rect.top > nv21_height
        || rect.bottom<0 || rect.bottom > nv21_height
        || rect.left<0 || rect.left>nv21_width
        || rect.right<0 || rect.right>nv21_width)
    {
        DEBUG("rect error[%d, %d, %d, %d] !!!\n", rect.left, rect.top, rect.right, rect.bottom);
        ret = -1;
        goto EXIT;
    }
      
    x_offset = (rect.right - rect.left) * (face_rect_scale) / 2;
    y_offset = (rect.bottom - rect.top) * (face_rect_scale) / 2;

    if((rect.left - x_offset) >= 0)
    {
        real_rect.left = rect.left - x_offset;
    }
    else
    {
        real_rect.left = 0;
    }

    if((rect.top - y_offset) >= 0)
    {
        real_rect.top = rect.top - y_offset;
    }
    else
    {
        real_rect.top = 0;
    }


    if((rect.right + x_offset) <= nv21_width)
    {
        real_rect.right = rect.right + x_offset;
    }
    else
    {
        real_rect.right = nv21_width;
    }

    if((rect.bottom + y_offset) <= nv21_height)
    {
        real_rect.bottom = rect.bottom + y_offset;
    }
    else
    {
        real_rect.bottom = nv21_height;
    }

    real_rect.left = (real_rect.left / 4) * 4;
    real_rect.right = (real_rect.right / 4) * 4;
    real_rect.top = (real_rect.top / 4) * 4;
    real_rect.bottom = (real_rect.bottom / 4) * 4;

    src_y = (char *)nv21_addr;
    src_uv = src_y + (nv21_width * nv21_height);
    width = real_rect.right - real_rect.left;
    height = real_rect.bottom - real_rect.top;

    int dst_image_size = width * height * 3 / 2;
    dst_image_data = (char *)malloc(sizeof(char) * dst_image_size);
    if(!dst_image_data)
    {
        DEBUG("malloc is error!!!\n");
        ret = MGVL0_RET_ERROR;
        goto EXIT;
    }

    dst_y = (char *)dst_image_data;
    dst_uv = dst_y + (width * height);

    for(row = real_rect.top, i = 0; row < real_rect.bottom; row++)
    {
        memcpy(dst_y + i * width, src_y + real_rect.left + row * nv21_width, width);
        i++;
    }
    
    for(row = real_rect.top / 2, i = 0; row < real_rect.bottom / 2; row++)
    {
        memcpy(dst_uv + i * width, src_uv + real_rect.left + row * nv21_width, width);
        i++;
    }

    ret = sdk_save_nv21_jpeg(file_path, dst_image_data, width, height);
EXIT:
    if(dst_image_data)
        free(dst_image_data);

    return ret;
}


