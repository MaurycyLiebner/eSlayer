#ifndef ESTRINGIDMAP_H
#define ESTRINGIDMAP_H

#include "eexceptions.h"

#include <cstring>
#include <map>
#include <string>

template <typename T>
class eStringIdMapVector {
public:
    template <typename ValueRef>
    struct eIterateType {
        std::string fName;
        int fId;
        ValueRef fValue;
    };

    template <typename ContType>
    class eIterator {
    public:
        eIterator(const int id, ContType& cont) :
            mId(id), mCont(cont) {
        }

        decltype(auto) operator*() {
            const auto name = mCont.name(mId);
            decltype(auto) value = mCont.get(mId);
            return eIterateType<decltype(value)>{name, mId, value};
        }

        eIterator& operator++() {
            mId++;
            return *this;
        }

        bool operator!=(const eIterator& other) const {
            return mId != other.mId;
        }
    private:
        int mId;
        ContType& mCont;
    };

    eIterator<eStringIdMapVector> begin() { return eIterator(0, *this); }
    eIterator<eStringIdMapVector> end() { return eIterator(mValues.size(), *this); }

    eIterator<const eStringIdMapVector> begin() const { return eIterator(0, *this); }
    eIterator<const eStringIdMapVector> end() const { return eIterator(mValues.size(), *this); }

    int nextId() const { return mValues.size(); }

    int add(const std::string& str, const T& value) {
        const int oldId = id(str);
        if(oldId != -1) {
            eRuntimeThrow("Duplicate \"" + str + "\".");
        }
        const int newId = mValues.size();
        mMap[str] = newId;
        mValues.emplace_back(value);
        return newId;
    }

    const T& get(const int id) const {
        return mValues[id];
    }

    T& get(const int id) {
        return mValues[id];
    }

    int id(const std::string& str) const {
        const auto it = mMap.find(str);
        if(it == mMap.end()) return -1;
        return it->second;
    }

    std::string name(const int id) const {
        for(const auto& it : mMap) {
            if(it.second == id) {
                return it.first;
            }
        }
        return "";
    }

    int size() const { return mValues.size(); }

    void reserve(const int size) { mValues.reserve(size); }

    void clear() {
        mMap.clear();
        mValues.clear();
    }
private:
    std::map<std::string, int> mMap;
    std::vector<T> mValues;
};

#endif // ESTRINGIDMAP_H
