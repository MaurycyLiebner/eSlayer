#ifndef ESLAYERMAPGENERATOREXPORT_H
#define ESLAYERMAPGENERATOREXPORT_H

#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef ESLAYERMAPGENERATOR_EXPORTS
        #define ESLAYERMAPGENERATOR_API __declspec(dllexport)
    #else
        #define ESLAYERMAPGENERATOR_API __declspec(dllimport)
#endif
#else
    #define ESLAYERMAPGENERATOR_API
#endif

#endif // ESLAYERMAPGENERATOREXPORT_H
