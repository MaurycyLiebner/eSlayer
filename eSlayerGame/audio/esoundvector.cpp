#include "esoundvector.h"

#include <eSlayerHelpers/erand.h>

void eSoundVector::play(MIX_Mixer * const mixer,
                        const std::shared_ptr<eTrackHolder>& track,
                        const int id,
                        const float volume,
                        const int loop,
                        const bool fadeIn) {
    const int count = eAudioVector::count();
    if(id < 0 || id >= count) return;
    const auto& a = audio(id);
    const auto playAudio = [mixer, track, loop, volume, fadeIn](
            MIX_Audio * const audio) {
        const auto mtrack = track->fTrack->fTrack;
        MIX_SetTrackAudio(mtrack, audio);
        MIX_SetTrackGain(mtrack, volume);

        const auto props = SDL_CreateProperties();
        SDL_SetNumberProperty(props,
            MIX_PROP_PLAY_LOOPS_NUMBER,
            loop);
        if(fadeIn) {
            SDL_SetNumberProperty(props,
                MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER,
                500);
        }

        MIX_PlayTrack(mtrack, props);

        SDL_DestroyProperties(props);
    };

    if(a) {
        playAudio(a);
    } else {
        loadAudio(mixer, id, playAudio);
    }
}

int eSoundVector::playRandomSound(MIX_Mixer * const mixer,
                                  const std::shared_ptr<eTrackHolder>& track,
                                  const float volume,
                                  const int loop,
                                  const bool fadeIn) {
    const int sc = count();
    if(sc <= 0) return -1;
    const int id = eRand::rand() % sc;
    play(mixer, track, id, volume, loop, fadeIn);
    return id;
}
