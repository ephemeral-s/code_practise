#pragma once
#include <atomic>
#include <utility> // std::swap

template<class T>
class mySharedPtr {
public:
    // 1. 构造函数：如果是空指针，不分配引用计数内存
    explicit mySharedPtr(T* ptr = nullptr) 
        : _ptr(ptr), _pcount(ptr ? new std::atomic<unsigned>(1) : nullptr) {}

    // 2. 拷贝构造
    mySharedPtr(const mySharedPtr<T>& p) : _ptr(p._ptr), _pcount(p._pcount) {
        if (_pcount) {
            (*_pcount)++;
        }
    }

    // 3. 移动构造（加分项：高效，无需修改引用计数）
    mySharedPtr(mySharedPtr<T>&& p) noexcept : _ptr(p._ptr), _pcount(p._pcount) {
        p._ptr = nullptr;
        p._pcount = nullptr;
    }

    // 4. 析构函数
    ~mySharedPtr() {
        release();
    }

    // 5. 赋值运算符重载（使用 Copy-and-Swap 统一处理拷贝和移动，极其优雅且异常安全）
    mySharedPtr<T>& operator=(mySharedPtr<T> rhs) noexcept {
        this->swap(rhs);
        return *this;
    }

    // 解引用运算符
    T& operator*() const { return *_ptr; }
    T* operator->() const { return _ptr; }
    T* get() const { return _ptr; }
    unsigned use_count() const { return _pcount ? _pcount->load() : 0; }

    // 辅助 swap 函数
    void swap(mySharedPtr<T>& p) noexcept {
        std::swap(_ptr, p._ptr);
        std::swap(_pcount, p._pcount);
    }

private:
    void release() {
        if (_pcount) {
            if (--(*_pcount) == 0) {
                delete _ptr;
                delete _pcount;
                _ptr = nullptr;
                _pcount = nullptr;
            }
        }
    }

    T* _ptr;
    std::atomic<unsigned>* _pcount;
};