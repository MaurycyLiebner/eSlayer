#ifndef EAUDIOVECTOR_H
#define EAUDIOVECTOR_H

#include <string>
#include <vector>

#include <SDL3_mixer/SDL_mixer.h>

class eAudioVector {
public:
    eAudioVector();
    ~eAudioVector();

    int soundCount() const { return mPaths.size(); }

    void addPath(MIX_Mixer * const mixer,
                 const std::string& path,
                 const bool load = false);
protected:
    MIX_Audio* loadAudio(
        MIX_Mixer * const mixer,
        const std::string& path);

    std::vector<std::pair<MIX_Audio*, std::string>> mPaths;
};

#endif // EAUDIOVECTOR_H
