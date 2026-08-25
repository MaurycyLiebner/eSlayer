#include "eaudiovector.h"

#include "../efileloader.h"

eThreadPool eAudioVector::sThreadPool(1);

eAudioVector::eAudioVector() {}

eAudioVector::eAudioVector(
    const std::vector<std::string>& paths) {
    for(const auto& path : paths) {
        mPaths.push_back({nullptr, path});
    }
}

eAudioVector::~eAudioVector() {
    waitUntilAllLoaded();
    for(const auto& s : mPaths) {
        if(!s.first) continue;
        MIX_DestroyAudio(s.first);
    }
}

void eAudioVector::loadAudio(
    MIX_Mixer * const mixer,
    const int id,
    const eFinishAudio& finish) {
    const auto it = mLoaders.find(id);
    if(it != mLoaders.end()) {
        if(finish) {
            auto& loader = it->second;
            loader->fFinish.emplace_back(finish);
        }
        return;
    }

    const auto loader = std::make_shared<eAudioLoader>();
    if(finish) loader->fFinish.emplace_back(finish);

    const auto audio = std::make_shared<MIX_Audio*>(nullptr);

    auto& p = mPaths[id];
    const auto work = [audio, mixer, &p]() {
        *audio = eFileLoader::loadAudio(
            mixer, "Audio", p.second);
    };
    const auto finishE = [this, audio, loader, &p, id](const std::exception_ptr& e) {
        mLoaders.erase(id);
        if(e) {
            try {
                std::rethrow_exception(e);
            } catch(const std::exception& e) {
                eRuntimeThrow("Error loading audio " + e.what());
            }
        }
        if(*audio) {
            p.first = *audio;
            for(const auto& finish : loader->fFinish) {
                finish(*audio);
            }
        }
    };
    sThreadPool.submit(work, finishE);
}

void eAudioVector::handleLoaded() {
    sThreadPool.update();
}

void eAudioVector::waitUntilAllLoaded() {
    sThreadPool.wait();
}

bool eAudioVector::loading(const int id) {
    return mLoaders.count(id) > 0;
}

MIX_Audio* eAudioVector::audio(const int id) const {
    return mPaths[id].first;
}
