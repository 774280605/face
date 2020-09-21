//
// Created by zxf on 9/18/20.
//

#ifndef ZBK_SERVICE_HTTPRESPONSE_H
#define ZBK_SERVICE_HTTPRESPONSE_H
#include <sstream>
#include <vector>
#include "../include/fsl0_common_struct.h"
class HttpResponse {
public:
    HttpResponse();
    virtual ~HttpResponse();

    void setCode(int code);

    void setReason(char* data);

    void setData(unsigned char *data, int len);

   virtual void getResponse(std::stringstream&stream);

protected:
    int m_code;
    unsigned char *m_data;

    char* m_reason;

};

class ErrResponse: public HttpResponse{
public:
    ErrResponse()=default;
    virtual ~ErrResponse()=default;


    void getResponse(std::stringstream &stream) override;


};



class CheckFaceResponse: public HttpResponse {
public:
    CheckFaceResponse(bool hasFace);
    virtual ~CheckFaceResponse()=default;

    void getResponse(std::stringstream &stream) override;

private:
    bool m_hasFace;
};

class ExtractFeatureResponse: public HttpResponse{
public:
    ExtractFeatureResponse(MGVL0_FEATURE_RESULT_ST*feature,int size);
    virtual ~ExtractFeatureResponse()=default;


    void getResponse(std::stringstream &stream) override;

private:
    static void encodeBase64(std::vector<std::string>&result,MGVL0_FEATURE_RESULT_ST*feature_lists,int feature_lists_size);
    static void base64ToJson(std::vector<std::string> &feature_lists, std::string &data);


    MGVL0_FEATURE_RESULT_ST*m_feature;
    int m_featureCount;
};


class CompareFeatureResponse: public HttpResponse{
public:
    CompareFeatureResponse(float score);

    virtual ~CompareFeatureResponse() = default;

    void getResponse(std::stringstream &stream) override;

private:
    float m_score;
};

#endif //ZBK_SERVICE_HTTPRESPONSE_H
