//
// Created by zxf on 9/10/20.
//

#include "Service.h"
#include <string>

#define SERVICE_DET_MODEL_FILE      "../models/M_det_x86_v1.2.bin"
#define SERVICE_FEATURE_MODEL_FILE  "../models/M_feature_x86_cusk_v1.2.bin"



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

    evhttp_set_cb(http_handle_, "/checkface", check_face_handler, this);
    evhttp_set_cb(http_handle_, "/extractfeature", extract_face_feature, this);
    evhttp_set_cb(http_handle_, "/comparefeature", compare_feature_handler, this);

    evhttp_set_gencb(http_handle_,default_handler,NULL);

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

void Service::default_handler(struct evhttp_request *req, void *arg) {
    evhttp_send_reply(req, HTTP_OK, "OK", NULL);
}

//find
int Service::search_eof(unsigned char*data,int size){
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



void Service::save_image(struct evbuffer *buf, int file_size, std::string file_name, int boundary_len) {



    FILE* file;

    file= fopen(file_name.c_str(),"wb+");
    int total_len = evbuffer_get_length(buf);



    unsigned char* jpeg= evbuffer_pullup(buf,total_len);

    int index= search_eof(jpeg,total_len);
    int verify_index = verify_file_size(total_len, file_size, boundary_len);
    if(index==verify_index)
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




void Service::check_face_handler(struct evhttp_request *req, void *arg) {
    auto service= (Service*)arg;
     if( evhttp_request_get_command(req)!=EVHTTP_REQ_POST){
         evhttp_send_reply(req, HTTP_OK, "only support post req", NULL);
     }
     struct evkeyvalq*list= evhttp_request_get_input_headers(req);

     const char* filesize= evhttp_find_header(list,"filesize");
    if (filesize == NULL) {
        evhttp_send_reply(req, HTTP_BADREQUEST, "file size is 0", NULL);
        return;
    }

    //print
    for(auto header= list->tqh_first;header;header=header->next.tqe_next)
    {
        printf("%s:  %s\n", header->key, header->value);
    }
    //Content-Type:  multipart/form-data; boundary=----WebKitFormBoundaryIEOWvJgT1sHJWp6e

    const char* tmp_boundary= evhttp_find_header(list,"Content-Type");
    auto boundary = boundary_len(tmp_boundary);






    const char *upload_file_name = evhttp_find_header(list, "filename");
    if(upload_file_name!=NULL)
        service->m_ImageFilePath= upload_file_name;



    int file_size = std::stoi(filesize);
    struct evbuffer* input;
    input = evhttp_request_get_input_buffer(req);
    service->save_image(input, file_size, service->m_ImageFilePath, boundary);
    auto has_face = service->detect_face(service->m_ImageFilePath);

    struct evkeyvalq*output_headers= evhttp_request_get_output_headers(req);
    evhttp_add_header(output_headers,"hasface",has_face==0?"true":"false");

    evhttp_send_reply(req, HTTP_OK, "OK", NULL);
    //delete template file
    ::remove(service->m_ImageFilePath.c_str());
}

//// 人脸是否满足各种阈值限制 0:正常 1:不满足 roll 2:不满足 yaw
////3:不满足 pitch 4:不满足 blur
//// 5:不满足 face_min 6:不满足 brightness 7:脸部未全部在图片
////范围内
int Service::detect_face(std::string image_path) {
    auto ret = sdk_detect_face(&m_sdk_handle, (char*)image_path.c_str());
    if(ret){
        DEBUG("sdk_detect_face error");
    }
    return ret;
}

MGVL0_FEATURE_RESULT_S * Service::extract_feature(std::string image_path, int &face_result_count) {
    MGVL0_FEATURE_RESULT_S *feature_result = NULL;

    int feature_result_count = 0;
    auto ret = sdk_detect_face_get_feature(&m_sdk_handle, (char*)image_path.c_str(), &feature_result, &feature_result_count);
    if(ret)
    {
        DEBUG("sdk_detect_face_get_feature error !!!\n");

    }
    face_result_count=feature_result_count;
    return feature_result;
}

float Service::compare_feature(char *featureA, int lenA, char *featureB, int lenB) {
    MGVL0_FEATURE_RESULT_S featureA_result={.feature_data=featureA,.feature_length=lenA};
    MGVL0_FEATURE_RESULT_S featureB_result={.feature_data=featureB,.feature_length= lenB};


    float compare_result = 0.0;
    auto ret = mgvl0_compare_feature(m_sdk_handle.compare_handle, &featureA_result, &featureB_result, &compare_result);
    if(ret)
    {
        DEBUG("mgvl0_compare_feature error !!!\n");
        ret = DEMO_FAIL;

    }
    else
    {
        DEBUG("compare_1:1 scores=%f\n", compare_result);
    }
    return compare_result;
}

void Service::extract_face_feature(struct evhttp_request *req, void *arg) {
    auto service= (Service*)arg;
    if( evhttp_request_get_command(req)!=EVHTTP_REQ_POST){
        evhttp_send_reply(req, HTTP_OK, "only support post req", NULL);
    }
    struct evkeyvalq*list= evhttp_request_get_input_headers(req);

    const char* filesize= evhttp_find_header(list,"filesize");
    if (filesize == NULL) {
        evhttp_send_reply(req, HTTP_BADREQUEST, "file size is 0", NULL);
        return;
    }

    const char* tmp_boundary= evhttp_find_header(list,"Content-Type");
    auto boundary = boundary_len(tmp_boundary);

    const char *upload_file_name = evhttp_find_header(list, "filename");
    if(upload_file_name!=NULL)
        service->m_ImageFilePath= upload_file_name;



    int file_size = std::stoi(filesize);
    struct evbuffer* input;
    input = evhttp_request_get_input_buffer(req);
    service->save_image(input, file_size, service->m_ImageFilePath, boundary);
    int feature_result_count=0;
    auto feature = service->extract_feature(service->m_ImageFilePath, feature_result_count);

    struct evbuffer* output= evhttp_request_get_output_buffer(req);
    evbuffer_add(output,feature->feature_data,feature->feature_length);

    struct evkeyvalq*output_headers= evhttp_request_get_output_headers(req);
    evhttp_add_header(output_headers,"featuresize",std::to_string(feature->feature_length).c_str());
    evhttp_add_header(output_headers,"feature_result_count",std::to_string(feature_result_count).c_str());

    evhttp_send_reply(req, HTTP_OK, "OK", NULL);
    ::remove(service->m_ImageFilePath.c_str());
    //release feature
    mgvl0_delete(feature);
}

void Service::compare_feature_handler(struct evhttp_request *req, void *arg) {
    auto service= (Service*)arg;
    if( evhttp_request_get_command(req)!=EVHTTP_REQ_POST){
        evhttp_send_reply(req, HTTP_OK, "only support post req", NULL);
    }

    struct evkeyvalq*list= evhttp_request_get_input_headers(req);

    const char *feature_a_len = evhttp_find_header(list, "feature_a_len");
    const char *feature_b_len = evhttp_find_header(list, "feature_b_len");
    if (feature_b_len == nullptr ||feature_a_len == nullptr) {
        evhttp_send_reply(req, HTTP_BADREQUEST, "feature param error!", NULL);
    }
    int lenA= std::stoi(feature_a_len);
    int lenB= std::stoi(feature_b_len);
    struct evbuffer* input_buffer= evhttp_request_get_input_buffer(req);
    if(evbuffer_get_length(input_buffer)!=(lenA+lenB)){
        evhttp_send_reply(req, HTTP_BADREQUEST, "PARAM ERROR!", NULL);
    }


    unsigned char *data = evbuffer_pullup(input_buffer, evbuffer_get_length(input_buffer));


    float score= service->compare_feature((char*)data, lenA,
                                          (char*)data + lenA, std::stoi(feature_b_len));

    struct evkeyvalq*output_headers= evhttp_request_get_output_headers(req);
    evhttp_add_header(output_headers,"score",std::to_string(score).c_str());


    evhttp_send_reply(req, HTTP_OK, "OK", NULL);

}
