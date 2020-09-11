#pragma once

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "sdk_config.h"
#include "fsl0_interface.h"
#include "fsl0_detect.h"
#include "fsl0_feature.h"
#include "fsl0_compare.h"

#include "libjpeg_api.h"
#include "comm_func.h"

typedef struct SDK_HANDLE_ST
{
    MGVL0_HANDLE_T track_handle;
    MGVL0_HANDLE_T select_face_handle;
	MGVL0_HANDLE_T detect_handle;
	MGVL0_HANDLE_T feature_handle;
	MGVL0_HANDLE_T compare_handle;
    MGVL0_HANDLE_T search_handle;
}SDK_HANDLE_S;

int add_one_feature_to_txt(char *txt_path, COMM_FACE_INFO_S *face_info);
int read_feature_from_txt(char *txt_path, int max_count, COMM_FACE_GROUP_S *face_group);

int sdk_detect_face(SDK_HANDLE_S *sdk_handle, char *image_file);

int sdk_detect_face_get_feature(SDK_HANDLE_S *sdk_handle, char *image_file, MGVL0_FEATURE_RESULT_S **feature_result, int *feature_result_count);

int sdk_detect_face_feat_to_simple_group(SDK_HANDLE_S *sdk_handle, DEMO_FEATURE_SAVE_TYPE_E type, 
                                        char *jpg_folder, 
                                        char *group_folder, COMM_FACE_GROUP_S **face_group);

int read_face_group(char *group_folder, int max_count, COMM_FACE_GROUP_S **face_group);

int sdk_compare_result_save(COMM_FACE_GROUP_S *face_group_query, COMM_FACE_GROUP_S *face_group_base, 
                            MGVL0_FEATURE_COMPARE_RESULT_S *result, char *output);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
