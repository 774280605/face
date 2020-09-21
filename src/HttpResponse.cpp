//
// Created by zxf on 9/18/20.
//

#include "HttpResponse.h"
#include "encode.h"
#include <algorithm>
void HttpResponse::setReason(char *data) {
    m_reason=data;
}

void HttpResponse::setCode(int code) {
    this->m_code=code;
}

HttpResponse::HttpResponse() {

}

HttpResponse::~HttpResponse() {

}

void HttpResponse::setData(unsigned char *data, int len) {
    this->m_data=data;
}
/*
 * {
 *      "code":0,
 *      "reason":"success",
 *      "data":{
 *          "hasface":true
 *      }
 *
 *
 * }
 * */
void HttpResponse::getResponse(std::stringstream&stream) {
    stream<<"{";
    stream<<"\"code\":"<<this->m_code<<",";//eg:"code":0
    stream<<"\"reason\":"<<"\""<<m_reason<<"\""<<",";
    stream<<"\"data\":"<<"[";
    stream<<m_data;
    stream<<"]";
    stream<<"}";
}

void CheckFaceResponse::getResponse(std::stringstream &stream) {
    stream<<"{";
    stream<<"\"code\":"<<0<<",";//eg:"code":0
    stream<<"\"reason\":"<<"\""<<"success"<<"\""<<",";
    stream<<"\"data\":"<<"{";
    stream<<"\"hasface\":";
    stream<<m_hasFace;
    stream<<"}";
    stream<<"}";
}

CheckFaceResponse::CheckFaceResponse(bool hasFace):m_hasFace(hasFace) {


}

void ExtractFeatureResponse::getResponse(std::stringstream &stream) {


    std::vector<std::string> feature_lists;
    encodeBase64(feature_lists, m_feature, m_featureCount);

    std::string json_data;
    base64ToJson(feature_lists,json_data);


    stream<<"{";
    stream<<"\"code\":"<<0<<",";//eg:"code":0
    stream<<"\"reason\":"<<"\""<<"success"<<"\""<<",";
    stream<<"\"data\":"<<"[";
    stream<<json_data;
    stream<<"]";
    stream<<"}";
}

ExtractFeatureResponse::ExtractFeatureResponse(MGVL0_FEATURE_RESULT_ST *feature, int size):
        m_feature(feature), m_featureCount(size){

}

void ExtractFeatureResponse::encodeBase64(std::vector<std::string> &result, MGVL0_FEATURE_RESULT_ST *feature_lists,
                                          int feature_lists_size) {

    for (int i = 0; i < feature_lists_size; ++i) {
        base64::encoder ec;
        base64_init_encodestate(&ec._state);
        std::vector<char> buffer(feature_lists[i].feature_length<<1);
        auto count = ec.encode(feature_lists[i].feature_data, feature_lists[i].feature_length, buffer.data());
        count += ec.encode_end(buffer.data() + count);
        result.push_back(std::string(buffer.data(),buffer.data()+count));
    }

}

void ExtractFeatureResponse::base64ToJson(std::vector<std::string> &feature_lists, std::string &data) {
    std::stringstream ss;
    for (int i = 0; i < feature_lists.size(); ++i) {
        ss << "{" << "\"featureLen\":" << feature_lists[i].length() << ",";
        ss << "\"data\":" << "\"" << feature_lists[i].c_str() << "\""<<"},";
    }

    data= ss.str();
    data.erase(data.length() - 1);
    data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
}

void ErrResponse::getResponse(std::stringstream &stream) {
    stream<<"{";
    stream<<"\"code\":"<<-1<<",";//eg:"code":0
    stream<<"\"reason\":"<<"\""<<m_reason<<"\""<<",";
    stream<<"\"data\":"<<"{";
    stream<<"}";
    stream<<"}";
}

CompareFeatureResponse::CompareFeatureResponse(float score):m_score(score) {

}

void CompareFeatureResponse::getResponse(std::stringstream &stream) {
    stream<<"{";
    stream<<"\"code\":"<<0<<",";//eg:"code":0
    stream<<"\"reason\":"<<"\""<<"success"<<"\""<<",";
    stream<<"\"data\":"<<"{";
    stream<<"\"score\":";
    stream<<m_score;
    stream<<"}";
    stream<<"}";
}
