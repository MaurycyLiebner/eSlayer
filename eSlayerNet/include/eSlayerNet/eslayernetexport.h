#ifndef ESLAYERNETEXPORT_H
#define ESLAYERNETEXPORT_H

#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef ESLAYERNET_EXPORTS
        #define ESLAYERNET_API __declspec(dllexport)
    #else
        #define ESLAYERNET_API __declspec(dllimport)
#endif
#else
    #define ESLAYERNET_API
#endif

#endif // ESLAYERNETEXPORT_H
