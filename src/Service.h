//
// Created by zxf on 9/10/20.
//

#ifndef FACE_QUALITY_SERVICE_H
#define FACE_QUALITY_SERVICE_H

#include <evhttp.h>
#include <event.h>
#include <string>
#include <vector>
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

    static void DefaultHandler(struct evhttp_request*req, void*arg);
    static void CheckFaceHandler(struct evhttp_request*req, void*arg);
    static void ExtractFaceFeature(struct evhttp_request*req, void*arg);

    static void CompareFeatureHandler(struct evhttp_request *req, void *arg);

    int DetectFace(std::string image_path);
    int ExtractFeature(std::string image_path, int &face_result_count, MGVL0_FEATURE_RESULT_S *&face_result);
    int CompareFeature(char *featureA, int lenA, char *featureB, int lenB, float &score);

    static void EncodeBase64(std::vector<std::string>&result, MGVL0_FEATURE_RESULT_ST*feature_lists, int feature_lists_size);
   // static void Base64TRRoJson(std::vector<std::string> &feature_lists, std::string &data);
    static int DecodeBase64(char*data, int in_len, char*out);
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
    void saveImage(struct evbuffer *buf, int file_size, std::string file_name, int boundary_len);
    //search client upload image`s range
    int searchEof(unsigned char*data, int size);

    unsigned char m_featureA[516];
    unsigned char m_featureB[516];
    int m_count{0};
};


#endif //FACE_QUALITY_SERVICE_H
