#ifndef EIDMAPVECTOR_H
#define EIDMAPVECTOR_H

#include <vector>
#include <map>
#include <set>
#include <memory>

template <typename T>
class eIdMapVector {
public:
    using eCont = std::vector<std::shared_ptr<T>>;

    template <typename ContType>
    class eIterator {
    public:
        eIterator(const int id, ContType& cont) :
            mId(id), mCont(cont) {
            skipEmpty();
        }

        decltype(auto) operator*() {
            return mCont[mId];
        }

        eIterator& operator++() {
            mId++;
            skipEmpty();
            return *this;
        }

        bool operator!=(const eIterator& other) const {
            return mId != other.mId;
        }
    private:
        void skipEmpty() {
            while(mId < mCont.size() && !mCont[mId]) {
                mId++;
            }
        }

        int mId;
        ContType& mCont;
    };

    eIterator<eCont> begin() { return eIterator(0, mValues); }
    eIterator<eCont> end() { return eIterator(mValues.size(), mValues); }

    eIterator<const eCont> begin() const { return eIterator(0, mValues); }
    eIterator<const eCont> end() const { return eIterator(mValues.size(), mValues); }

    void add(const int id, const std::shared_ptr<T>& v) {
        int vid;
        if(mSlots.empty()) {
            vid = mValues.size();
            mValues.emplace_back(v);
        } else {
            const auto it =  mSlots.extract(mSlots.begin());
            vid = it.value();
        }
        mIdMap[id] = vid;
    }

    bool remove(const int id) {
        const auto it = mIdMap.find(id);
        if(it == mIdMap.end()) return false;
        const int vid = it->second;
        mValues[vid] = nullptr;
        mSlots.emplace(vid);
        return true;
    }

    std::shared_ptr<T> get(const int id) const {
        const auto it = mIdMap.find(id);
        if(it == mIdMap.end()) return nullptr;
        const int vid = it->second;
        return mValues[vid];
    }

    const eCont& get() const {
        return mValues;
    }
private:
    eCont mValues;
    std::map<int, int> mIdMap;
    std::set<int> mSlots;
};

#endif // EIDMAPVECTOR_H
