#ifndef ESLAYERSERVEREXPORT_H
#define ESLAYERSERVEREXPORT_H

#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef ESLAYERSERVER_EXPORTS
        #define ESLAYERSERVER_API __declspec(dllexport)
    #else
        #define ESLAYERSERVER_API __declspec(dllimport)
#endif
#else
    #define ESLAYERSERVER_API
#endif

#endif // ESLAYERSERVEREXPORT_H
