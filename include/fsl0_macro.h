#pragma once

#ifdef __cplusplus
    #define MGVL0_API_BEGIN extern "C" {
    #define MGVL0_API_END }
#else
    #define MGVL0_API_BEGIN
    #define MGVL0_API_END
#endif

#if MGVL0_LIBRARY
    #if defined(_WIN32)
        #if MGVL0_DLLEXPORT
            #define MGVL0_API __declspec(dllexport)
        #else
            #define MGVL0_API __declspec(dllimport)
        #endif
    #else
        #if MGVL0_DLLEXPORT
            #define MGVL0_API __attribute__((visibility("default")))
        #else
            #define MGVL0_API
        #endif
    #endif
#else
    #define MGVL0_API
#endif

#ifdef __GNUC__
    #define MGVL0_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
    #define MGVL0_DEPRECATED __declspec(deprecated)
#else
    #define MGVL0_DEPRECATED
#endif
