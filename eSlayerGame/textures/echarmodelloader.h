#ifndef ECHARMODELLOADER_H
#define ECHARMODELLOADER_H

#include "espriteloader.h"
#include "echarmodel.h"

#include <functional>

using eFinished = std::function<void(const std::shared_ptr<eCharModel>&)>;

class eSpriteLoaderLoader {
public:
    void set(const std::shared_ptr<eSpriteLoader>& loader);
    void load();
    void finish();

    void addFinish(const std::function<void()>& finish);
private:
    std::shared_ptr<eSpriteLoader> mLoader;
    std::vector<std::function<void()>> mFinished;
};

class eCharModelLoader {
public:
    eCharModelLoader(const std::shared_ptr<eCharModel>& model);

    const std::shared_ptr<eCharModel>& model() const { return mModel; }

    void addLoader(const std::shared_ptr<eSpriteLoaderLoader>& loader);

    bool empty() const { return mRemaining <= 0; }

    void addFinish(const eFinished& finish);
    void setFinishDelete(const eFinished& finish);
private:
    int mRemaining = 0;

    std::shared_ptr<eCharModel> mModel;
    std::vector<eFinished> mFinished;
    eFinished mFinishDelete;
};

#endif // ECHARMODELLOADER_H
