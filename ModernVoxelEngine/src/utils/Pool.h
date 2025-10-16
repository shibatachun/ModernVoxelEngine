
#pragma once

#include <assert.h>
#include <cstdint>
#include <vector>

namespace utils {


    template<typename ObjectType>
    class Handle final {
    public:
        Handle() = default;

        bool empty() const {
            return gen_ == 0;
        }
        bool valid() const {
            return gen_ != 0;
        }
        uint32_t index() const {
            return index_;
        }
        uint32_t gen() const {
            return gen_;
        }
        void* indexAsVoid() const {
            return reinterpret_cast<void*>(static_cast<ptrdiff_t>(index_));
        }
        bool operator==(const Handle<ObjectType>& other) const {
            return index_ == other.index_ && gen_ == other.gen_;
        }
        bool operator!=(const Handle<ObjectType>& other) const {
            return index_ != other.index_ || gen_ != other.gen_;
        }
        // allow conditions 'if (handle)'
        explicit operator bool() const {
            return gen_ != 0;
        }

    private:
        Handle(uint32_t index, uint32_t gen) : index_(index), gen_(gen) {};

        template<typename ObjectType_, typename ImplObjectType>
        friend class Pool;

        uint32_t index_ = 0;
        uint32_t gen_ = 0;
    };

    template<typename ObjectType, typename ImplObjectType>
    class Pool {
        static constexpr uint32_t kListEndSentinel = 0xffffffff;
        struct PoolEntry {
            explicit PoolEntry(ImplObjectType& obj) : obj_(std::move(obj)) {}
            ImplObjectType obj_ = {};
            uint32_t gen_ = 1;
            uint32_t nextFree_ = kListEndSentinel;
        };
        uint32_t freeListHead_ = kListEndSentinel;
        uint32_t numObjects_ = 0;

    public:
        std::vector<PoolEntry> objects_;

        Handle<ObjectType> create(ImplObjectType&& obj) {
            uint32_t idx = 0;
            if (freeListHead_ != kListEndSentinel) {
                idx = freeListHead_;
                freeListHead_ = objects_[idx].nextFree_;
                objects_[idx].obj_ = std::move(obj);
            }
            else {
                idx = (uint32_t)objects_.size();
                objects_.emplace_back(obj);
            }
            numObjects_++;
            return Handle<ObjectType>(idx, objects_[idx].gen_);
        }
        void destroy(Handle<ObjectType> handle) {
            if (handle.empty())
                return;
            assert(numObjects_ > 0); // double deletion
            const uint32_t index = handle.index();
            assert(index < objects_.size());
            assert(handle.gen() == objects_[index].gen_); // double deletion
            objects_[index].obj_ = ImplObjectType{};
            objects_[index].gen_++;
            objects_[index].nextFree_ = freeListHead_;
            freeListHead_ = index;
            numObjects_--;
        }
        const ImplObjectType* get(Handle<ObjectType> handle) const {
            if (handle.empty())
                return nullptr;

            const uint32_t index = handle.index();
            assert(index < objects_.size());
            assert(handle.gen() == objects_[index].gen_); // accessing deleted object
            return &objects_[index].obj_;
        }
        ImplObjectType* get(Handle<ObjectType> handle) {
            if (handle.empty())
                return nullptr;

            const uint32_t index = handle.index();
            assert(index < objects_.size());
            assert(handle.gen() == objects_[index].gen_); // accessing deleted object
            return &objects_[index].obj_;
        }
        Handle<ObjectType> getHandle(uint32_t index) const {
            assert(index < objects_.size());
            if (index >= objects_.size())
                return {};

            return Handle<ObjectType>(index, objects_[index].gen_);
        }
        Handle<ObjectType> findObject(const ImplObjectType* obj) {
            if (!obj)
                return {};

            for (size_t idx = 0; idx != objects_.size(); idx++) {
                if (objects_[idx].obj_ == *obj) {
                    return Handle<ObjectType>((uint32_t)idx, objects_[idx].gen_);
                }
            }

            return {};
        }
        void clear() {
            objects_.clear();
            freeListHead_ = kListEndSentinel;
            numObjects_ = 0;
        }
        uint32_t numObjects() const {
            return numObjects_;
        }
    };
}