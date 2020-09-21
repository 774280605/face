#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../src/ErrorCode.h"
#include "demo_comm.h"
#include "sdk_comm_api.h"

int add_one_feature_to_txt(char *txt_path, COMM_FACE_INFO_S *face_info)
{
    int  j = 0;
    int  ret = MGVL0_RET_ERROR;
    FILE *fp = NULL;

    if(!txt_path || !face_info)
    {
        DEBUG("input param error\n");
        return MGVL0_RET_ERROR;
    }

    fp = fopen(txt_path, "a");
    if(fp)
    {
        fprintf(fp, "jpg_path: %s\n", face_info->jpg_path);
        fprintf(fp, "feat_size:%d\n", face_info->feature_size);
        fprintf(fp, "feat: ");
        for(j=0; j<face_info->feature_size; j++)
        {
            fprintf(fp, "%02x ", (unsigned char)face_info->feature[j]);
        }
        fprintf(fp, "\n\n");
    }
    else
    {
        ret = MGVL0_RET_ERROR;
        DEBUG("fopen error\n");
        goto EXIT;
    }

    ret = MGVL0_RET_OK;
EXIT:
    if(fp)
    {
        fclose(fp);
    }
    return ret;
}

int  read_feature_from_txt(char *txt_path, int max_count, COMM_FACE_GROUP_S *face_group)
{
    int ret = DEMO_FAIL;
    char *line_buf = NULL;
    int i = 0;
    FILE *fp = NULL;
    char *p = NULL;
    if(!txt_path || max_count <= 0 || max_count > MAX_FACE_COUNT || !face_group)
    {
        DEBUG("input param error\n");
        ret = DEMO_FAIL;
        goto EXIT;
    }
    
    line_buf = (char *)malloc(32 * 1024);
    if(!line_buf)
    {
        DEBUG("malloc error\n");
        ret = DEMO_FAIL;
        goto EXIT;
    }

    face_group->face_count = 0;

    fp = fopen(txt_path, "r");
    if(!fp)
    {
        DEBUG("fopen[%s] error\n", txt_path);
        ret = DEMO_FAIL;
        goto EXIT;        
    }
    
    
    while(fgets(line_buf, 32 * 1024, fp))
    {
        if(strstr(line_buf, "jpg_path:"))
        {
            ret = sscanf(line_buf, "jpg_path:%s", face_group->face[face_group->face_count].jpg_path);
        }
        else if(strstr(line_buf, "feat_size:"))
        {
            ret = sscanf(line_buf, "feat_size:%d", &face_group->face[face_group->face_count].feature_size);
            assert(face_group->face[face_group->face_count].feature_size <= FEATURE_DATA_LEN);
        }
        else if(strstr(line_buf, "feat:"))
        {
            p = line_buf + strlen("feat:");
			
			assert(face_group->face[face_group->face_count].feature);
			
            for(i = 0; i < face_group->face[face_group->face_count].feature_size && p < line_buf + 32 * 1024; i++)
            {
                int tmp32 = 0;
                ret = sscanf(p, "%02x", &tmp32);
                if(ret > 0)
                {
                    face_group->face[face_group->face_count].feature[i] = (char)tmp32;
                }
                else
                {
                    break;
                }
                p += 3;
            }

            if(i >= face_group->face[face_group->face_count].feature_size)
            {
                face_group->face_count++;
                if(face_group->face_count >= max_count)
                {
                    break;
                }
            }
        }
    }
    ret = MGVL0_RET_OK;
EXIT:
    if(fp)
    {
        fclose(fp);
        fp = NULL;
    }
        
    if(line_buf)
    {
        free(line_buf);
        line_buf = NULL;
    }
    return ret;
}

int get_jpg_count_from_feat_txt(char * feat_txt_path)
{
    int jpg_count = 0;
    char *line_buf = NULL;
    FILE *fp = NULL;

    if(!feat_txt_path)
    {
        DEBUG("input param error\n");
        goto EXIT;
    }
    
    line_buf = (char *)malloc(32 * 1024);
    if(!line_buf)
    {
        DEBUG("malloc error\n");
        goto EXIT;
    }


    fp = fopen(feat_txt_path, "r");
    if(!fp)
    {
        DEBUG("fopen[%s] error\n", feat_txt_path);
        goto EXIT;        
    }
    
    while(fgets(line_buf, 32 * 1024, fp))
    {
        if(strstr(line_buf, "jpg_path:"))
        {
            jpg_count++;
        }
    }

EXIT:
    if(fp)
    {
        fclose(fp);
        fp = NULL;
    }
        
    if(line_buf)
    {
        free(line_buf);
        line_buf = NULL;
    }
    
    return jpg_count;
}

int read_face_group(char *group_folder, int max_count, COMM_FACE_GROUP_S **face_group)
{
    int ret = DEMO_FAIL;
    char txt_path[256] = {};
    int i=0, jpg_count=0;
    COMM_FACE_GROUP_S *p_face_group = NULL;
    if(!group_folder || max_count <= 0 || max_count > MAX_FACE_COUNT || !face_group)
    {
        DEBUG("input param error\n");
        return DEMO_FAIL;
    }
    *face_group = NULL;
    
    snprintf(txt_path, sizeof(txt_path) - 1, "%s/%s", group_folder, FEAT_TXT_NAME);
    jpg_count = get_jpg_count_from_feat_txt(txt_path);
    p_face_group = (COMM_FACE_GROUP_S  *)malloc(sizeof(COMM_FACE_GROUP_S) + jpg_count*FEATURE_DATA_LEN);
    if(NULL == p_face_group)
    {
        DEBUG("malloc error\n");
        ret = DEMO_FAIL;
        goto EXIT;
    }
    else
    {
        memset(p_face_group, 0, sizeof(COMM_FACE_GROUP_S) + jpg_count*FEATURE_DATA_LEN);
        p_face_group->face_count = 0;
        for(i = 0; i < jpg_count; i++)
        {
            p_face_group->face[i].feature = (char *)p_face_group + sizeof(COMM_FACE_GROUP_S) + i*FEATURE_DATA_LEN;
        }
    }
    ret = read_feature_from_txt(txt_path, max_count, p_face_group);
    
EXIT:
    if(ret != DEMO_OK)
        free(p_face_group);
    else
        *face_group = p_face_group;

    return ret;
}

int sdk_compare_result_save(COMM_FACE_GROUP_S *face_group_query, COMM_FACE_GROUP_S *face_group_base, MGVL0_FEATURE_COMPARE_RESULT_S *result, char *output)
{
    int ret = DEMO_FAIL;
    char filepath[DEMO_FILE_LEN] = {0};
    char filename[DEMO_FILE_LEN*4] = {0};
    char src_image_name[DEMO_FILE_LEN] = {0};
    char jpg_name[DEMO_FILE_LEN] = {0};
    char *pos = NULL;
    char sys_cmd[DEMO_FILE_LEN*2] = {0};
    if(!face_group_query || !face_group_base || !result || !output)
    {
        DEBUG("input param error\n");
        ret = DEMO_FAIL;
        goto EXIT;
    }

	int i = 0, j = 0;

#if 1
    mkdir(output, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    snprintf(filepath, sizeof(filepath) - 1, "%s/COMPARE_RESULT", output);
    mkdir(filepath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    snprintf(sys_cmd, sizeof(sys_cmd), "rm %s/* -rf", filepath);
    system(sys_cmd);
#endif

    for(i = 0; i < result->score_list_size; i++)
    {
        printf("--------------------------\n");
        printf("capture jpg: %s\n", face_group_query->face[i].jpg_path);
        printf("base jpg: \n");
        for(j = 0; j < result->score_list[i].top_k; j++)
        {
            printf("%s, score: %.1f \n", face_group_base->face[result->score_list[i].face_id[j]].jpg_path,
                                    result->score_list[i].scores[j]);
        }
        printf("\n");
    }

#if 1
    for(i = 0; i < result->score_list_size; i++)
    {
        if(result->score_list[i].scores[0] < DEMO_COMPARE_SCORE)
        {
            continue;
        }
        
        strncpy(src_image_name, basename(face_group_query->face[i].jpg_path), sizeof(jpg_name) - 1);
        snprintf(filename, sizeof(filename), "%s/%s", filepath, src_image_name);
        copy_file(face_group_query->face[i].jpg_path, filename);
        pos = strstr(src_image_name, ".jpg");
        if(pos)
        {
            *pos = 0;
        }

        for(j = 0; j < result->score_list[i].top_k; j++)
        {
            int index = result->score_list[i].face_id[j];
            strncpy(jpg_name, basename(face_group_base->face[index].jpg_path), sizeof(jpg_name) - 1);
            pos = strstr(jpg_name, ".jpg");
            if(pos)
            {
                *pos = 0;
            }

            snprintf(filename, sizeof(filename) - 1, "%s/%s_%d_g_%s_score%.1f.jpg", filepath, src_image_name,
                     j, jpg_name, result->score_list[i].scores[j]);

            copy_file(face_group_base->face[index].jpg_path, filename);
        }
    }
#endif

    ret = DEMO_OK;
EXIT:
    return ret;
}

int sdk_save_face_jpeg(MGVL0_FACE_IMAGE_S *face_image, const char* output_jpg_name)
{
    return sdk_save_nv21_jpeg((char *)output_jpg_name, (char *)face_image->image.data, face_image->image.width, face_image->image.height);
}


int sdk_save_data(char*filename, void *buf,int size)
{
    int  ret = DEMO_FAIL;
    FILE *fp = NULL;
    fp = fopen(filename, "w");
    if(fp)
    {
        fwrite(buf, size, 1, fp);
        fclose(fp);
        ret = DEMO_OK;
    }
    
    return ret;
}

int sdk_detect_face(SDK_HANDLE_S *sdk_handle, char *image_file)
{
	int  ret = DEMO_FAIL;
    MGVL0_FRAME_S frame = {0};
	MGVL0_FACE_DETECT_RESULT_S *detect_result = NULL;
	int i = 0;
	
    ret = sdk_read_jpeg(image_file, &frame.image);
	if(ret)
	{
		DEBUG("sdk_read_jpeg error !!!\n"); 
		ret = ERR_READ_JPEG;
		goto EXIT;
	}
    
	MGVL0_FACE_DETECT_CONFIG_S detect_config = {};
	detect_config.face_min = 50;
	detect_config.pose_roll_upper_threshold  = 30;
	detect_config.pose_yaw_upper_threshold   = 30;
	detect_config.pose_pitch_upper_threshold = 30;
	detect_config.blurriness_upper_threshold = 0.7;
    detect_config.brightness_low_threshold = 70;
    detect_config.brightness_upper_threshold = 210;
    detect_config.brightness_deviation_threshold = 60;
    detect_config.face_completeness_threshold = 0.9;
	frame.frame_id  = 1;
	frame.frame_pts = 1;
	frame.image.addr_type = MGVL0_IMAGE_SOURCE_DATA;
	frame.image.rotation = MGVL0_ROTATION_DEG0;
	frame.image.format	 = MGVL0_IMAGE_NV21;
	ret = mgvl0_detect_face(sdk_handle->detect_handle, &detect_config, &frame, &detect_result);
	if(ret)
	{
		DEBUG("mgvl0_detect_face error !!!\n"); 
		ret = ERR_DETECT_FACE;
		goto EXIT;
	}

	if(detect_result->face_list_size <= 0)
	{
		DEBUG("mgvl0_detect_face face count = 0 !!!\n"); 
        mgvl0_delete(detect_result);
		ret = ERR_FACE_LIST_NULL;
		goto EXIT;
	}

	for(i = 0; i < detect_result->face_list_size; i++)
	{
		if(0 == detect_result->face_list[i].errcode)
		{
            DEBUG("%s quality ok  ", image_file); 
		}
		else
		{
			DEBUG("%s quality bad, errcode = %d ", image_file, detect_result->face_list[i].errcode); 
		}
        printf("rect[%d %d %d %d], roll=%f pitch=%f, yaw=%f, blur=%f, face brightness=%d, brightness_deviation=%d, completeness=%f\n",
            detect_result->face_list[i].rect.left,
            detect_result->face_list[i].rect.top,               
            detect_result->face_list[i].rect.right,
            detect_result->face_list[i].rect.bottom,
            detect_result->face_list[i].pose.roll,
            detect_result->face_list[i].pose.pitch,
            detect_result->face_list[i].pose.yaw,
            detect_result->face_list[i].blur,
            detect_result->face_list[i].brightness,
            detect_result->face_list[i].brightness_deviation,
            detect_result->face_list[i].face_completeness
            ); 
        
	}
    
	ret = DEMO_OK;
EXIT:
    if(frame.image.data)
    {
        free(frame.image.data);
    }

	if(detect_result)
    {
        mgvl0_delete(detect_result);
        detect_result = NULL;
    }

	return ret;
}

int sdk_detect_face_get_feature(SDK_HANDLE_S *sdk_handle, char *image_file, MGVL0_FEATURE_RESULT_S **feature_result, int *feature_result_count)
{
	int  ret = DEMO_FAIL;
    MGVL0_FRAME_S frame = {0};
	MGVL0_FACE_DETECT_RESULT_S *detect_result = NULL;
	MGVL0_FACE_IMAGE_S *face_image = NULL;
	int i = 0;
	
	int face_real_count = 0;

    ret = sdk_read_jpeg(image_file, &frame.image);
	if(ret)
	{
		DEBUG("sdk_read_jpeg error !!!\n"); 
		ret = ERR_READ_JPEG;
		goto EXIT;
	}
    
	MGVL0_FACE_DETECT_CONFIG_S detect_config = {};
	detect_config.face_min = 50;
	detect_config.pose_roll_upper_threshold  = 30;
	detect_config.pose_yaw_upper_threshold   = 30;
	detect_config.pose_pitch_upper_threshold = 30;
	detect_config.blurriness_upper_threshold = 0.7;
    detect_config.brightness_low_threshold = 70;
    detect_config.brightness_upper_threshold = 210;
    detect_config.brightness_deviation_threshold = 60; 
    detect_config.face_completeness_threshold = 0.9;
	frame.frame_id  = 1;
	frame.frame_pts = 1;
	frame.image.addr_type = MGVL0_IMAGE_SOURCE_DATA;
	frame.image.rotation = MGVL0_ROTATION_DEG0;
	frame.image.format	 = MGVL0_IMAGE_NV21;
	ret = mgvl0_detect_face(sdk_handle->detect_handle, &detect_config, &frame, &detect_result);
	if(ret)
	{
		DEBUG("mgvl0_detect_face error !!!\n"); 
		ret = ERR_DETECT_FACE;
		goto EXIT;
	}

	if(detect_result->face_list_size <= 0)
	{
		DEBUG("mgvl0_detect_face face count = 0 !!!\n"); 
        mgvl0_delete(detect_result);
		ret = ERR_FACE_LIST_NULL;
		goto EXIT;
	}

	face_image = (MGVL0_FACE_IMAGE_S *)malloc(sizeof(MGVL0_FACE_IMAGE_S) * detect_result->face_list_size);
	if(NULL == face_image)
	{
		DEBUG("malloc error !!!\n"); 
		ret = ERR_MALLOC_MEMORY;
		goto EXIT;
	}

	for(i = 0; i < detect_result->face_list_size; i++)
	{
		if(0 == detect_result->face_list[i].errcode)
		{
			memcpy(&face_image[face_real_count].image, &frame.image, sizeof(MGVL0_IMAGE_S));
			face_image[face_real_count].rect.left   = detect_result->face_list[i].rect.left;
			face_image[face_real_count].rect.top    = detect_result->face_list[i].rect.top;
			face_image[face_real_count].rect.right  = detect_result->face_list[i].rect.right;
			face_image[face_real_count].rect.bottom = detect_result->face_list[i].rect.bottom;

			face_image[face_real_count].lmk.landmark_length = detect_result->face_list[i].landmark.landmark_length;
			face_image[face_real_count].lmk.landmark_data = detect_result->face_list[i].landmark.landmark_data;

			face_real_count++;
		}
		else
		{
			DEBUG("%s quality bad, errcode = %d ", image_file, detect_result->face_list[i].errcode); 
            printf("rect[%d %d %d %d], roll=%f pitch=%f, yaw=%f, blur=%f, face brightness=%d, brightness_deviation=%d, completeness=%f\n",
                detect_result->face_list[i].rect.left,
                detect_result->face_list[i].rect.top,               
                detect_result->face_list[i].rect.right,
                detect_result->face_list[i].rect.bottom,
                detect_result->face_list[i].pose.roll,
                detect_result->face_list[i].pose.pitch,
                detect_result->face_list[i].pose.yaw,
                detect_result->face_list[i].blur,
                detect_result->face_list[i].brightness,
                detect_result->face_list[i].brightness_deviation,
                detect_result->face_list[i].face_completeness
                ); 
		}
	}
    
    if(face_real_count > 0)
	{
		ret = mgvl0_extract_feature(sdk_handle->feature_handle, face_image, face_real_count, feature_result);
		if(ret)
		{
			DEBUG("mgvl0_extract_feature error !!!\n"); 
			ret = ERR_EXTRACT_FEATURE;
			goto EXIT;
		}
		else 
		{
			*feature_result_count = face_real_count;
			DEBUG_TRACE("mgvl0_extract_feature %s OK: feature_result_count = %d\n", 
					image_file, *feature_result_count);
		}
	}
	else
	{
		*feature_result_count = 0;
        DEBUG("high quality face count = 0 !!!\n"); 
        ret = ERR_FACE_LIST_NULL;
        goto EXIT;
	}

	ret = DEMO_OK;
EXIT:
    if(frame.image.data)
    {
        free(frame.image.data);
    }

	if(detect_result)
    {
        mgvl0_delete(detect_result);
        detect_result = NULL;
    }

	if(face_image)
	{
		free(face_image);
		face_image = NULL;
	}

	return ret;
}

int get_jpg_count_in_folder(char *jpg_folder)
{
    int jpg_count = 0;
    DIR *dir_jpg = NULL;
    struct dirent *ptr1 = NULL;

    if(jpg_folder == NULL)
    {
        goto EXIT;
    }

    dir_jpg = opendir(jpg_folder);
    if (dir_jpg == NULL)
    {
        DEBUG("open folder error: %s\n", strerror(errno));
        goto EXIT;
    }

    while ((ptr1 = readdir(dir_jpg)) != NULL)
    {
        if(strstr(ptr1->d_name, ".jpg") || strstr(ptr1->d_name, ".jpeg"))
        {
            jpg_count++;
        }
    }

EXIT:
    if(dir_jpg)
    {
        closedir(dir_jpg);
        dir_jpg = NULL;
    }

    return jpg_count;
}

int sdk_detect_face_feat_to_simple_group(SDK_HANDLE_S *sdk_handle, DEMO_FEATURE_SAVE_TYPE_E type, 
                                        char *jpg_folder, 
                                        char *group_folder, COMM_FACE_GROUP_S **face_group)
{
    int   i = 0;
    int   ret = DEMO_FAIL;
    int   jpg_count = 0;
    char  feat_txt_file[DEMO_FILE_LEN] = {};

    char  sys_cmd[DEMO_PATH_LEN] = {};
    char  jpg_path[DEMO_PATH_LEN] = {};
    DIR   *dir_jpg = NULL;
    struct dirent *ptr1 = NULL;

    int group_face_count = 0;
    
    MGVL0_FEATURE_RESULT_S *feature_result = NULL;
    int feature_result_count = 0;

    COMM_FACE_GROUP_S       *ptr_face_group = NULL;
    COMM_FACE_INFO_S        face_info = {};

    if(!sdk_handle || !jpg_folder || !group_folder)
    {
        DEBUG("input is NULL !!!\n");
        ret = DEMO_FAIL;
        goto EXIT;
    }

    snprintf(feat_txt_file, sizeof(feat_txt_file)-1, "%s/%s", group_folder, FEAT_TXT_NAME);

    //Get the number of JPG pictures in jpg_folder
    jpg_count = get_jpg_count_in_folder(jpg_folder);
    if(jpg_count <= 0)
    {
        DEBUG("jpg_folder[%s] have %d jpgs\n", jpg_folder, jpg_count);
        ret = DEMO_FAIL;
        goto EXIT;
    }

	face_info.feature = (char *)malloc(FEATURE_DATA_LEN);
	if(NULL == face_info.feature)
	{
		DEBUG("malloc error\n");
        ret = DEMO_FAIL;
        goto EXIT;
	}

	if(SAVE_TO_MEM == type)
	{
	    ptr_face_group = (COMM_FACE_GROUP_S  *)malloc(sizeof(COMM_FACE_GROUP_S) + jpg_count*FEATURE_DATA_LEN);
	    if(NULL == ptr_face_group)
	    {
	        DEBUG("malloc error\n");
	        ret = DEMO_FAIL;
	        goto EXIT;
	    }
		else
		{
	    	memset(ptr_face_group, 0, sizeof(COMM_FACE_GROUP_S) + jpg_count*FEATURE_DATA_LEN);
			ptr_face_group->face_count = 0;
			for(i = 0; i < jpg_count; i++)
			{
				ptr_face_group->face[i].feature = (char *)ptr_face_group + sizeof(COMM_FACE_GROUP_S) + i*FEATURE_DATA_LEN;
			}
		}
	}

    if(SAVE_TO_FILE == type)       
    {
        snprintf(sys_cmd, sizeof(sys_cmd), "rm %s/* -rf", group_folder);
        system(sys_cmd);
    }

    dir_jpg = opendir(jpg_folder);
    while ((ptr1 = readdir(dir_jpg)) != NULL)
    {
        if ((strcmp(ptr1->d_name, ".") == 0) || (strcmp(ptr1->d_name, "..") == 0)
            || strlen(ptr1->d_name) == 0)
        {
            continue;
        }
        if(ptr1->d_type == DT_DIR)
        {
            continue;
        }

        if(strstr(ptr1->d_name, ".jpg") == NULL
			&& strstr(ptr1->d_name, ".jpeg") == NULL
			&& strstr(ptr1->d_name, ".JPG") == NULL
			&& strstr(ptr1->d_name, ".JPEG") == NULL)
        {
            continue;
        }

        sprintf(jpg_path, "%s/%s", jpg_folder, ptr1->d_name);
        ret = sdk_detect_face_get_feature(sdk_handle, jpg_path, &feature_result, &feature_result_count);
        if(ret)
        {
            DEBUG("sdk_detect_face_get_feature %s error !!!\n", jpg_path); 
            continue;
        }

        if(feature_result_count > 1)
        {
            DEBUG("many face in one photo: %s , use first only!!!\n", jpg_path); 
        }
        
        face_info.feature_size = feature_result->feature_length;
        memcpy(face_info.feature, feature_result->feature_data, feature_result->feature_length);
        
        if(SAVE_TO_FILE == type)        // Save feature information to text file
        {   
            snprintf(face_info.jpg_path, sizeof(face_info.jpg_path)-1, "%s/%04d.jpg",
                                    group_folder, ++group_face_count);
            add_one_feature_to_txt(feat_txt_file, &face_info);
            copy_file(jpg_path, face_info.jpg_path);
        }
		
        if(SAVE_TO_MEM == type)         // Save feature information to ptr_face_group
        {   
            strncpy(face_info.jpg_path, jpg_path, DEMO_FILE_LEN - 1);
            //memcpy(&(ptr_face_group->face[ptr_face_group->face_count++]), &face_info, sizeof(COMM_FACE_INFO_S));
            memcpy(ptr_face_group->face[ptr_face_group->face_count].jpg_path, face_info.jpg_path, sizeof(face_info.jpg_path));
			memcpy(ptr_face_group->face[ptr_face_group->face_count].feature, face_info.feature, face_info.feature_size);
			ptr_face_group->face[ptr_face_group->face_count].feature_size = face_info.feature_size;
			ptr_face_group->face_count++;
        }

        mgvl0_delete(feature_result);
    }

    if(type == SAVE_TO_MEM)
    {
        *face_group = ptr_face_group;
    }
    ret = MGVL0_RET_OK;

EXIT:
	if(face_info.feature)
	{
		free(face_info.feature);
		face_info.feature = NULL;
	}

    if(dir_jpg)
    {
        closedir(dir_jpg);
    }
    return ret;
}
