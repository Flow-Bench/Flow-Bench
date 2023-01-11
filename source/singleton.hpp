#pragma once

// a singleton class template

#include <memory>

namespace flowbench {

template <class T>
class Singleton {
private:
    static std::unique_ptr<T> instance;

public:
    static T& getInstance() {
        if (instance == nullptr) {
            instance = std::make_unique<T>();
        }
        return *instance;
    }

    template <typename... Args>
    static void setInstance(Args&&... args) {
        instance = std::make_unique<T>(std::forward<Args>(args)...);
    }

    Singleton(const Singleton& other) = delete;
    Singleton(Singleton&& other) = delete;
    Singleton& operator=(const Singleton& other) = delete;
    Singleton& operator=(Singleton&& other) = delete;

protected:
    Singleton() = default;
    virtual ~Singleton() = default;

};

template <class T>
std::unique_ptr<T> Singleton<T>::instance = nullptr;

}