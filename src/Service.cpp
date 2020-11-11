//
// Created by zxf on 9/10/20.
//

#include "Service.h"
#include <string>
#include "HttpResponse.h"
#include "encode.h"
#include "cencode.h"
#include <algorithm>
#include "ErrorCode.h"
#include "decode.h"
#define SERVICE_DET_MODEL_FILE      "./models/meg_x86_detctor_C_fp_v1.3.bin"
#define SERVICE_FEATURE_MODEL_FILE  "./models/meg_x86_feature_H_fp_v1.8.bin"



Service::~Service() {
    mgvl0_destroy_detect_handle(m_sdk_handle.detect_handle);
    mgvl0_destroy_feature_handle(m_sdk_handle.feature_handle);
    mgvl0_destroy_compare_handle(m_sdk_handle.compare_handle);
    mgvl0_del_model(m_model);
    mgvl0_deinit_sdk();
}

Service::Service() {
    //malloc(10000);
    //malloc_stats_print(NULL,NULL,NULL);
}

void Service::init() {
    this->sdk_init();
    this->http_init();

}

void Service::run() {
    event_dispatch();
}

void Service::http_init() {
    event_init();
    http_handle_= evhttp_start("0.0.0.0",9527);
    if(http_handle_==NULL)
        exit(-1);

    evhttp_set_cb(http_handle_, "/checkface", CheckFaceHandler, this);
    evhttp_set_cb(http_handle_, "/extractfeature", ExtractFaceFeature, this);
    evhttp_set_cb(http_handle_, "/comparefeature", CompareFeatureHandler, this);

    evhttp_set_gencb(http_handle_, DefaultHandler, NULL);

}

void Service::sdk_init() {
    memset(&m_sdk_param,0,sizeof(MGVL0_SDK_PARAM_S));
    //
    auto ret = mgvl0_init_sdk(&m_sdk_param);
    if(ret){
        DEBUG("mgv10_init_sdk error");
        return;
    }
    const char*version= nullptr;
    mgvl0_get_version(&version);
    DEBUG("sdk version  = %s\n", version);
    mgvl0_set_log_level(0);

    m_cfg.type = MGVL0_MODEL_TYPE_DETECT;
    ret = mgvl0_add_model(SERVICE_DET_MODEL_FILE, &m_cfg, &m_model);
    if(ret){
        DEBUG("load detect model error");
        return ;
    }

    m_cfg.type=MGVL0_MODEL_TYPE_FEATURE;
    ret = mgvl0_add_model(SERVICE_FEATURE_MODEL_FILE, &m_cfg, &m_model);
    if(ret){
        DEBUG("load feature model error");
        return ;
    }

    ret = mgvl0_create_detect_handle(m_model, &m_sdk_handle.detect_handle);
    if(ret){
        DEBUG("create detect handle error");
        return;
    }

    ret = mgvl0_create_feature_handle(m_model, &m_sdk_handle.feature_handle);
    if (ret) {
        DEBUG("create feature handle error");
        return;
    }

    ret = mgvl0_create_compare_handle(m_model, &m_sdk_handle.compare_handle);
    if (ret) {
        DEBUG("create compare error");
        return;
    }


}

void Service::DefaultHandler(struct evhttp_request *req, void *arg) {
    evhttp_send_reply(req, HTTP_OK, "OK", NULL);
}

//find
int Service::searchEof(unsigned char*data, int size){
    for(int i=0;i<size;++i){
        if(data[i]=='\r' && data[i+1]=='\n'&&
           data[i+2]=='\r' && data[i+3]=='\n')
        {
            return i+4;
        }
    }
    return -1;
}

int verify_file_size(int data_len,int file_size,int boundary_len)
{
    auto index = data_len-file_size-boundary_len-4-4;
    return index;
}



void Service::saveImage(struct evbuffer *buf, int file_size, std::string file_name, int boundary_len) {



    FILE* file;

    file= fopen(file_name.c_str(),"wb+");
    int total_len = evbuffer_get_length(buf);



    unsigned char* jpeg= evbuffer_pullup(buf,total_len);

    int index= searchEof(jpeg, total_len);
    //int verify_index = verify_file_size(total_len, file_size, boundary_len);
    if(index != -1)
        fwrite(jpeg+index, 1, file_size, file);


    fclose(file);
}

int boundary_len(const char* boundary)
{
    std::string tmp(boundary);
    auto index = tmp.rfind("boundary=");
    if (index == std::string::npos) {
        return -1;
    }

    return tmp.length()-index-9;
}

/*{
    "code":0
    "data":
    "reason":"success"
}*/


void Service::CheckFaceHandler(struct evhttp_request *req, void *arg) {
    auto service= (Service*)arg;
     if( evhttp_request_get_command(req)!=EVHTTP_REQ_POST){
         evhttp_send_reply(req, HTTP_OK, "only support post req", NULL);
     }


     std::stringstream stream;
    struct evbuffer* output= evhttp_request_get_output_buffer(req);
     struct evkeyvalq*list= evhttp_request_get_input_headers(req);
     const char* filesize= evhttp_find_header(list,"filesize");
    if (filesize == NULL) {
        ErrResponse errResponse;
        errResponse.setReason("filesize is NULL");
        errResponse.getResponse(stream);
        evbuffer_add(output, stream.str().c_str(), stream.str().length());
        evhttp_send_reply(req, HTTP_OK, NULL, NULL);
        return;
    }

    //print
    for(auto header= list->tqh_first;header;header=header->next.tqe_next)
    {
        printf("%s:  %s\n", header->key, header->value);
    }
    //Content-Type:  multipart/form-data; boundary=----WebKitFormBoundaryIEOWvJgT1sHJWp6e

    const char* tmp_boundary= evhttp_find_header(list,"Content-Type");
    int boundary = boundary_len(tmp_boundary);

    const char *upload_file_name = evhttp_find_header(list, "filename");
    if(upload_file_name!=NULL)
        service->m_ImageFilePath= upload_file_name;
    else    {
        ErrResponse errResponse;
        errResponse.setReason("filename is NULL");
        errResponse.getResponse(stream);
        evbuffer_add(output, stream.str().c_str(), stream.str().length());
        evhttp_send_reply(req, HTTP_OK, NULL, NULL);
        return;
    }


    int file_size = std::stoi(filesize);
    struct evbuffer* input;
    input = evhttp_request_get_input_buffer(req);
    if(evbuffer_get_length(input)==0){
        ErrResponse errResponse;
        errResponse.setReason("file data is NULL");
        errResponse.getResponse(stream);
        evbuffer_add(output, stream.str().c_str(), stream.str().length());
        evhttp_send_reply(req, HTTP_OK, NULL, NULL);
        return;
    }





    service->saveImage(input, file_size, service->m_ImageFilePath, boundary);
    int has_face = service->DetectFace(service->m_ImageFilePath);
    if(has_face<0){
        ErrResponse errResponse;
        errResponse.setReason(getReason(has_face));
        errResponse.getResponse(stream);


        evbuffer_add(output, stream.str().c_str(), stream.str().length());

        evhttp_send_reply(req, HTTP_OK, NULL, NULL);
        return;
    }
    CheckFaceResponse checkFaceResponse(has_face==0?true:false);

    checkFaceResponse.getResponse(stream);
   const char*str_data=stream.str().c_str();

   evbuffer_add(output, stream.str().c_str(), stream.str().length());

    evhttp_send_reply(req, HTTP_OK, NULL, NULL);
    //delete template file
    ::remove(service->m_ImageFilePath.c_str());
}

//// 人脸是否满足各种阈值限制 0:正常 1:不满足 roll 2:不满足 yaw
////3:不满足 pitch 4:不满足 blur
//// 5:不满足 face_min 6:不满足 brightness 7:脸部未全部在图片
////范围内
int Service::DetectFace(std::string image_path) {
    int ret = sdk_detect_face(&m_sdk_handle, (char*)image_path.c_str());
    if(ret){
        DEBUG("sdk_detect_face error");
    }
    return ret;
}

int Service::ExtractFeature(std::string image_path, int &face_result_count, MGVL0_FEATURE_RESULT_S *&face_result) {


    int feature_result_count = 0;
    auto ret = sdk_detect_face_get_feature(&m_sdk_handle, (char*)image_path.c_str(), &face_result, &feature_result_count);
    if(ret)
    {
        DEBUG("sdk_detect_face_get_feature error !!!\n");
    }
    face_result_count=feature_result_count;
    return ret;
}

int Service::CompareFeature(char *featureA, int lenA, char *featureB, int lenB, float &score) {
    char feature_decode_a[4096] = {0};
    char feature_decode_b[4096] = {0};
    int feature_decode_a_len= DecodeBase64(featureA,lenA,feature_decode_a);
    int feature_decode_b_len=DecodeBase64(featureB, lenB, feature_decode_b);
   /* {
        FILE* t=  fopen("./featureb","wb+");
        fwrite(feature_decode_a, feature_decode_a_len, 1, t);
        fclose(t);
    }*/


    MGVL0_FEATURE_RESULT_S featureA_result={.feature_data=feature_decode_a,.feature_length=feature_decode_a_len};
    MGVL0_FEATURE_RESULT_S featureB_result={.feature_data=feature_decode_b,.feature_length= feature_decode_b_len};


    float compare_result = 0.0;
    int ret = mgvl0_compare_feature(m_sdk_handle.compare_handle, &featureA_result, &featureB_result, &compare_result);
    if(ret)
    {
        DEBUG("mgvl0_compare_feature error !!!\n");
        ret = ERR_COMPARE_FEATURE;

    }
    else
    {
        DEBUG("compare_1:1 scores=%f\n", compare_result);
    }
    score=compare_result;
    return ret;
}

void Service::ExtractFaceFeature(struct evhttp_request *req, void *arg) {
    auto service= (Service*)arg;
    if( evhttp_request_get_command(req)!=EVHTTP_REQ_POST){
        evhttp_send_reply(req, HTTP_OK, "only support post req", NULL);
    }


    std::stringstream stream;
    struct evbuffer* output= evhttp_request_get_output_buffer(req);
    struct evkeyvalq*list= evhttp_request_get_input_headers(req);

    const char* filesize= evhttp_find_header(list,"filesize");
    if (filesize == NULL) {
        ErrResponse errResponse;

        errResponse.setReason("filesize is NULL");
        errResponse.getResponse(stream);
        evbuffer_add(output, stream.str().c_str(), stream.str().length());
        evhttp_send_reply(req, HTTP_OK, NULL, NULL);
        return;
    }
    ////
    //struct evbuffer*buf= evhttp_request_get_input_buffer(req);
    //int total_len= evbuffer_get_length(buf);
    //unsigned char* jpeg= evbuffer_pullup(buf,total_len);
    ////
    const char* tmp_boundary= evhttp_find_header(list,"Content-Type");
    int boundary = boundary_len(tmp_boundary);

    const char *upload_file_name = evhttp_find_header(list, "filename");
    if(upload_file_name!=NULL)
        service->m_ImageFilePath= upload_file_name;
    else
    {
        ErrResponse errResponse;
        errResponse.setReason("filename is NULL");
        errResponse.getResponse(stream);
        evbuffer_add(output, stream.str().c_str(), stream.str().length());
        evhttp_send_reply(req, HTTP_OK, NULL, NULL);
        return;
    }


    int file_size = std::stoi(filesize);
    struct evbuffer* input;
    input = evhttp_request_get_input_buffer(req);
    if(evbuffer_get_length(input)==0){
        ErrResponse errResponse;
        errResponse.setReason("file data is NULL");
        errResponse.getResponse(stream);
        evbuffer_add(output, stream.str().c_str(), stream.str().length());
        evhttp_send_reply(req, HTTP_OK, NULL, NULL);
        return;
    }

    service->saveImage(input, file_size, service->m_ImageFilePath, boundary);
    int feature_result_count=0;
    MGVL0_FEATURE_RESULT_ST* feature_result= nullptr;


    int ret = service->ExtractFeature(service->m_ImageFilePath, feature_result_count, feature_result);
    //test
   {
        FILE* t=  fopen("./feature","wb");
        fwrite(feature_result->feature_data, feature_result->feature_length, 1, t);
        fclose(t);

    }
    if (feature_result_count <= 0) {
        ErrResponse errResponse;

        errResponse.setReason("no face");

        errResponse.getResponse(stream);

        evbuffer_add(output, stream.str().c_str(), stream.str().length());
        evhttp_send_reply(req, HTTP_OK, NULL, NULL);
        ::remove(service->m_ImageFilePath.c_str());
    }else
    {
        ExtractFeatureResponse extractFeatureResponse(feature_result,feature_result_count);
        extractFeatureResponse.getResponse(stream);

        evbuffer_add(output, stream.str().c_str(), stream.str().length());

        evhttp_send_reply(req, HTTP_OK, NULL, NULL);
        ::remove(service->m_ImageFilePath.c_str());
        //release feature
        mgvl0_delete(feature_result);
    }



}

void Service::CompareFeatureHandler(struct evhttp_request *req, void *arg) {
    auto service= (Service*)arg;
    if( evhttp_request_get_command(req)!=EVHTTP_REQ_POST){
        evhttp_send_reply(req, HTTP_OK, "only support post req", NULL);
    }
    std::stringstream stream;
    struct evbuffer* output= evhttp_request_get_output_buffer(req);
    struct evkeyvalq*list= evhttp_request_get_input_headers(req);

    const char *feature_a_len = evhttp_find_header(list, "feature_a_len");
    const char *feature_b_len = evhttp_find_header(list, "feature_b_len");
    if (feature_b_len == nullptr ||feature_a_len == nullptr) {
        ErrResponse errResponse;
        errResponse.setReason("feature param error!");
        errResponse.getResponse(stream);
        evbuffer_add(output, stream.str().c_str(), stream.str().length());
        evhttp_send_reply(req, HTTP_BADREQUEST, NULL, NULL);
        return ;
    }
    int lenA= std::stoi(feature_a_len);
    int lenB= std::stoi(feature_b_len);
    struct evbuffer* input_buffer= evhttp_request_get_input_buffer(req);
    int input_len=evbuffer_get_length(input_buffer);


    if(input_len<(lenA+lenB) || input_len==0){
        ErrResponse errResponse;
        errResponse.setReason("PARAM ERROR!");
        errResponse.getResponse(stream);
        evbuffer_add(output, stream.str().c_str(), stream.str().length());
        evhttp_send_reply(req, HTTP_BADREQUEST, NULL, NULL);
        return;
    }

    unsigned char *data = evbuffer_pullup(input_buffer, evbuffer_get_length(input_buffer));
    float score=0.0;

    int ret= service->CompareFeature((char *) data, lenA,
                                     (char *) data + lenA, lenB, score);

    if(ret!=0){
        ErrResponse errResponse;
        errResponse.setReason(getReason(ret));
        errResponse.getResponse(stream);
        evbuffer_add(output, stream.str().c_str(), stream.str().length());
        evhttp_send_reply(req, HTTP_OK, NULL, NULL);
        return;
    }

    CompareFeatureResponse compareFeatureResponse(score);
    compareFeatureResponse.getResponse(stream);

    evbuffer_add(output, stream.str().c_str(), stream.str().length());
    //float score = service->compare_feature((char*)service->m_featureA,516,(char*)service->m_featureB,516);


    evhttp_send_reply(req, HTTP_OK, NULL, NULL);

}

void Service::EncodeBase64(std::vector<std::string> &result, MGVL0_FEATURE_RESULT_ST *feature_lists,
                           int feature_lists_size) {
    for (int i = 0; i < feature_lists_size; ++i) {
        base64::encoder ec;
        base64_init_encodestate(&ec._state);
        std::vector<char> buffer(feature_lists[i].feature_length<<1);
        int count = ec.encode(feature_lists[i].feature_data, feature_lists[i].feature_length, buffer.data());
        count += ec.encode_end(buffer.data() + count);
        result.push_back(std::string(buffer.data(),buffer.data()+count));
    }

}
/*
 * {
		"feature_len": 123,
		"data": "zzzzzzzzzz"
	},

	{
		"feature_len": 123,
		"data": "zzzzzzzzzz"
	},
	{
		"feature_len": 123,
		"data": "zzzzzzzzzz"
	}
 * */


int Service::DecodeBase64(char *data, int in_len, char *out) {
    base64::decoder decoder;
    base64_init_decodestate(&decoder._state);
    int len=  decoder.decode(data, in_len, out);
    return len;
}
