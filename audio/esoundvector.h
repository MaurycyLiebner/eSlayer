#ifndef ESOUNDVECTOR_H
#define ESOUNDVECTOR_H

#include <string>
#include <vector>

#include <SDL3_mixer/SDL_mixer.h>

class eSoundVector {
public:
    ~eSoundVector();

    int soundCount() const { return mPaths.size(); }
    void addPath(MIX_Mixer * const mixer,
                 const std::string& path,
                 const bool load = false);
    void play(MIX_Mixer * const mixer,
              const int id,
              MIX_Track * const channel = nullptr);
    void playRandomSound(MIX_Mixer * const mixer,
                         MIX_Track * const channel = nullptr);
private:
    std::vector<std::pair<MIX_Audio*, std::string>> mPaths;
};

#endif // ESOUNDVECTOR_H
