#ifndef EMUSICVECTOR_H
#define EMUSICVECTOR_H

#include <SDL3_mixer/SDL_mixer.h>

#include <vector>
#include <string>

class eMusicVector {
public:
    ~eMusicVector();

    int soundCount() const { return mPaths.size(); }
    void addPath(MIX_Mixer * const mixer,
                 const std::string& path,
                 const bool load = false);
    void play(MIX_Mixer * const mixer,
              MIX_Track * const track,
              const int id);
    void playRandomSound(MIX_Mixer * const mixer,
                         MIX_Track * const track);
private:
    std::vector<std::pair<MIX_Audio*, std::string>> mPaths;
};

#endif // EMUSICVECTOR_H
