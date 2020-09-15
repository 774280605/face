//
// Created by zxf on 9/10/20.
//

#ifndef FACE_QUALITY_SERVICE_H
#define FACE_QUALITY_SERVICE_H

#include <evhttp.h>
#include <event.h>
#include <string>
#include "demo_comm.h"
#include "sdk_config.h"
#include "sdk_comm_api.h"

#include "fsl0_interface.h"



#include <jemalloc/jemalloc.h>

class Service {
public:
    Service();
    ~Service();

    void init();
    void http_init();
    void sdk_init();
    void run();

    static void default_handler(struct evhttp_request*req,void*arg);
    static void check_face_handler(struct evhttp_request*req,void*arg);
    static void extract_face_feature(struct evhttp_request*req,void*arg);

    static void compare_feature_handler(struct evhttp_request *req, void *arg);

    int detect_face(std::string image_path);
    MGVL0_FEATURE_RESULT_S *extract_feature(std::string image_path, int &face_result_count);
    float compare_feature(char *featureA, int lenA, char *featureB, int lenB);
private:
    struct evhttp*http_handle_;
    MGVL0_SDK_PARAM_S m_sdk_param {};
    MGVL0_MODEL_T m_model{nullptr};
    MGVL0_MODEL_CFG_S m_cfg{};
    SDK_HANDLE_S m_sdk_handle{};
    //image path
    std::string m_ImageFilePath;

//private function
    //save client upload image
    void save_image(struct evbuffer *buf, int file_size, std::string file_name, int boundary_len);
    //search client upload image`s range
    int search_eof(unsigned char*data,int size);
};


#endif //FACE_QUALITY_SERVICE_H
