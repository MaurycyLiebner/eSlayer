#ifndef ESOUNDOPTIONS_H
#define ESOUNDOPTIONS_H

class eSoundOptions {
public:
    static int sMusicVolume;
    static int sSoundVolume;

    static void write();
    static bool read();
};

#endif // ESOUNDOPTIONS_H
