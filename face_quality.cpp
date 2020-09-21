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
#include <evhttp.h>
#include <event.h>
#include "demo_comm.h"
#include "sdk_config.h"
#include "sdk_comm_api.h"

#include "fsl0_interface.h"

#include "src/Service.h"


void usage(char **argv)
{
	printf("usage: %s [a:b]  \n"
          "       -a  face1.jpg\n"
          , argv[0]);
    printf("for example:\n"
           "%s -a face1.jpg\n",
           argv[0]);         
}











int main(int argc,char **argv)
{
    /*int   ret = DEMO_FAIL;
	int   c = 0;
	char  image1[DEMO_FILE_LEN+1] = {0};
    
	const char* version = NULL;
	MGVL0_SDK_PARAM_S sdk_param = {};
	MGVL0_MODEL_T model = NULL;
	MGVL0_MODEL_CFG_S cfg = {};
	SDK_HANDLE_S sdk_handle = {};*/



    Service service;
    service.init();
    service.run();



    return 0;


	/*if(argc < 3)
    {
        ret = DEMO_FAIL;
        usage(argv);
        goto EXIT;
    }

    //Parse parameter
    while((c = getopt(argc, argv, "a:")) != EOF)
    {
        switch(c)
        {
            case 'a':
                strncpy(image1, optarg, DEMO_FILE_LEN);
                break;
            default:
                break;    
        }
    }

	DEBUG("image1 = %s\n", image1);
    if(strlen(image1) == 0 ||
      (strstr(image1,".jpg") == NULL && strstr(image1,".jpeg") == NULL) )
    {
        ret = DEMO_FAIL;
        usage(argv);
        goto EXIT;
    }

    memset(&sdk_param,0,sizeof(MGVL0_SDK_PARAM_S));
    ret = mgvl0_init_sdk(&sdk_param);
    if(ret)
    {
        DEBUG("mgvl1_init_sdk error !!!\n"); 
        ret = DEMO_FAIL; 
        goto EXIT;
    }

    mgvl0_get_version(&version);
	DEBUG("sdk version  = %s\n", version); 
    mgvl0_set_log_level(0);

	cfg.type = MGVL0_MODEL_TYPE_DETECT;
	ret = mgvl0_add_model("/home/zxf/Desktop/work/test/models/M_det_x86_v1.2.bin"
	        *//*DET_MODEL_FILE*//*, &cfg, &model);
	if(ret)
	{
        DEBUG("mgvl0_add_model %s error !!!\n", DET_MODEL_FILE); 
        ret = DEMO_FAIL; 
        goto EXIT1;
	}

    ret = mgvl0_create_detect_handle(model, &sdk_handle.detect_handle);
	if(ret)
	{
		DEBUG("mgvl0_create_detector error !!!\n"); 
        ret = DEMO_FAIL; 
        goto EXIT2;
	}

	ret = sdk_detect_face(&sdk_handle, image1);
	if(ret)
	{
		DEBUG("sdk_detect_face_get_feature %s error !!!\n", image1); 
        ret = DEMO_FAIL; 
        goto EXIT3;
	}


	DEBUG("face quality test complete!!!\n"); 
	ret = DEMO_OK;
EXIT3:
	mgvl0_destroy_detect_handle(sdk_handle.detect_handle);
	
EXIT2:
	mgvl0_del_model(model);

EXIT1:
    mgvl0_deinit_sdk();

EXIT:
    return ret;*/
}
