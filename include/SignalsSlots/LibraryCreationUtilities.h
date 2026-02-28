#pragma once

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define DECL_EXPORT __declspec(dllexport)
    #define DECL_IMPORT __declspec(dllimport)
#else
    #define DECL_EXPORT __attribute__((visibility("default")))
    #define DECL_IMPORT __attribute__((visibility("default")))
#endif

#define LIBRARY_API DECL_EXPORT
#define LIBRARY_API_C_LINKAGE extern "C" DECL_EXPORT

#if defined(AS_STATIC_LIBRARY)
    #undef  LIBRARY_API
    #undef  LIBRARY_API_C_LINKAGE
    #define LIBRARY_API
    #define LIBRARY_API_C_LINKAGE
#endif

