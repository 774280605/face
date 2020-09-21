//
// Created by zxf on 9/20/20.
//

#ifndef ZBK_SERVICE_ERRORCODE_H
#define ZBK_SERVICE_ERRORCODE_H

enum ErrorCode{
    ERR_READ_JPEG=-1000,//
    ERR_DETECT_FACE=-1001,
    ERR_FACE_LIST_NULL=-1002,
    ERR_MALLOC_MEMORY=-1003,
    ERR_EXTRACT_FEATURE=-1004,
    ERR_COMPARE_FEATURE=-1005,
};




char*getReason(int code){
    switch (code) {
        case ERR_READ_JPEG:
            return "read jpeg file error";
        case ERR_DETECT_FACE:
            return "detect face error";
        case ERR_FACE_LIST_NULL:
            return "face list is null";
        case ERR_MALLOC_MEMORY:
            return "malloc error";
        case ERR_EXTRACT_FEATURE:
            return "extract feature error";
        case ERR_COMPARE_FEATURE:
            return "compare feature error";
        default:
            return "unknow error";
    }





}





#endif //ZBK_SERVICE_ERRORCODE_H
