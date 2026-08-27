#ifndef EAUDIOVECTOR_H
#define EAUDIOVECTOR_H

#include "../ethreadpool.h"

#include <string>
#include <vector>
#include <map>

#include <SDL3_mixer/SDL_mixer.h>

using eFinishAudio = std::function<void(MIX_Audio* const audio)>;

struct eAudioLoader {
    std::vector<eFinishAudio> fFinish;
};

class eTrack {
    friend struct eTrackHolder;
public:
    void initialize(MIX_Mixer* const mixer) {
        fTrack = MIX_CreateTrack(mixer);
    }

    bool used() const {
        return mUsed || MIX_TrackPlaying(fTrack);
    }

    MIX_Track* fTrack = nullptr;
private:
    bool mUsed = false;
};

struct eTrackHolder {
    eTrackHolder(const std::shared_ptr<eTrack>& track) {
        fTrack = track;
        fTrack->mUsed = true;
    }

    ~eTrackHolder() {
        fTrack->mUsed = false;
    }

    std::shared_ptr<eTrack> fTrack;
};

class eAudioVector {
public:
    eAudioVector();
    eAudioVector(const std::vector<std::string>& paths);
    ~eAudioVector();

    int count() const { return mPaths.size(); }
    bool loading(const int id);
    MIX_Audio* audio(const int id) const;

    void loadAudio(
        MIX_Mixer * const mixer,
        const int id,
        const eFinishAudio& finish);

    static void handleLoaded();
    static void waitUntilAllLoaded();
private:
    static eThreadPool sThreadPool;

    std::vector<std::pair<MIX_Audio*, std::string>> mPaths;
    std::map<int, std::shared_ptr<eAudioLoader>> mLoaders;
};

#endif // EAUDIOVECTOR_H
