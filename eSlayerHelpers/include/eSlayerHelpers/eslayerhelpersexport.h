#ifndef ESLAYERHELPERSEXPORT_H
#define ESLAYERHELPERSEXPORT_H

#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef ESLAYERHELPERS_EXPORTS
        #define ESLAYERHELPERS_API __declspec(dllexport)
    #else
        #define ESLAYERHELPERS_API __declspec(dllimport)
#endif
#else
    #define ESLAYERHELPERS_API
#endif

#endif // ESLAYERHELPERSEXPORT_H
