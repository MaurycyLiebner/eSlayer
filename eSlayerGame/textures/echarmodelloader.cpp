#include "echarmodelloader.h"

void eSpriteLoaderLoader::set(const std::shared_ptr<eSpriteLoader>& loader) {
    mLoader = loader;
}

void eSpriteLoaderLoader::load() {
    mLoader->initialize();
}

void eSpriteLoaderLoader::finish() {
    for(const auto& f : mFinished) {
        f();
    }
}

void eSpriteLoaderLoader::addFinish(const std::function<void()>& finish) {
    mFinished.emplace_back(finish);
}

eCharModelLoader::eCharModelLoader(
    const std::shared_ptr<eCharModel>& model) :
    mModel(model) {}

void eCharModelLoader::addLoader(const std::shared_ptr<eSpriteLoaderLoader>& loader) {
    mRemaining++;
    loader->addFinish([this]() {
        mRemaining--;
        if(mRemaining == 0) {
            mModel->clearCache();
            for(const auto& f : mFinished) {
                f(mModel);
            }
        }
    });
}

void eCharModelLoader::addFinish(const eFinished& finish) {
    mFinished.emplace_back(finish);
}
