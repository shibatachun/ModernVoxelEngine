#pragma once
#include <memory>
#include <mutex>

template <typename T>
class Singleton {
public:
    static T& GetInstance(auto&&... args) {
        std::call_once(initFlag, [&] {
            instance.reset(new T(std::forward<decltype(args)>(args)...));
            });
        return *instance;
    }

protected:
    Singleton() = default;
    virtual ~Singleton() = default;

private:
    static std::unique_ptr<T> instance;
    static std::once_flag initFlag;
};

template <typename T>
std::unique_ptr<T> Singleton<T>::instance = nullptr;

template <typename T>
std::once_flag Singleton<T>::initFlag;