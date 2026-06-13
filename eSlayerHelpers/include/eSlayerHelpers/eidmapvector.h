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
        eIterator(const uint32_t id, ContType& cont) :
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

        uint32_t mId;
        ContType& mCont;
    };

    eIterator<eCont> begin() { return eIterator(0, mValues); }
    eIterator<eCont> end() { return eIterator(mValues.size(), mValues); }

    eIterator<const eCont> begin() const { return eIterator(0, mValues); }
    eIterator<const eCont> end() const { return eIterator(mValues.size(), mValues); }

    void add(const uint32_t id, const std::shared_ptr<T>& v) {
        uint32_t vid;
        if(mSlots.empty()) {
            vid = mValues.size();
            mValues.emplace_back(v);
        } else {
            const auto it = mSlots.extract(mSlots.begin());
            vid = it.value();
            mValues[vid] = v;
        }
        mIdMap[id] = vid;
    }

    bool remove(const uint32_t id) {
        const auto it = mIdMap.find(id);
        if(it == mIdMap.end()) return false;
        const uint32_t vid = it->second;
        mValues[vid] = nullptr;
        mSlots.emplace(vid);
        mIdMap.erase(id);
        return true;
    }

    std::shared_ptr<T> get(const uint32_t id) const {
        const auto it = mIdMap.find(id);
        if(it == mIdMap.end()) return nullptr;
        const int vid = it->second;
        return mValues[vid];
    }

    const eCont& get() const {
        return mValues;
    }

    uint32_t size() const { return mValues.size(); }
    uint32_t actualSize() const { return mValues.size() - mSlots.size(); }

    void clear() {
        mValues.clear();
        mIdMap.clear();
        mSlots.clear();
    }
private:
    eCont mValues;
    std::map<uint32_t, uint32_t> mIdMap;
    std::set<uint32_t> mSlots;
};

#endif // EIDMAPVECTOR_H
