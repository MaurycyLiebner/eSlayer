#ifndef ERENDERSETTINGS_H
#define ERENDERSETTINGS_H

class eRenderSettings {
public:
    static bool sRenderObjectShadows;
    static bool sRenderWallShadows;

    static void write();
    static bool read();
};

#endif // ERENDERSETTINGS_H
