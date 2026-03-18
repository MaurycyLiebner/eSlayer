#ifndef ESLAYERMISSILESEXPORT_H
#define ESLAYERMISSILESEXPORT_H

#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef ESLAYERMISSILES_EXPORTS
        #define ESLAYERMISSILES_API __declspec(dllexport)
    #else
        #define ESLAYERMISSILES_API __declspec(dllimport)
#endif
#else
    #define ESLAYERMISSILES_API
#endif

#endif // ESLAYERMISSILESEXPORT_H
