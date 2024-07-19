// shared_memory_vec.h
#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <mutex>
#include <stdexcept>
#include <algorithm>
#include <array>


namespace _internal_shm {
    template <typename T, size_t N>
    class shared_memory_array {
    private:
        std::wstring name_;
        HANDLE h_map_file_;
        T* p_buf_;
        std::mutex mutex_;

    public:
        shared_memory_array(std::wstring name) : name_(std::move(name))
        {
            size_t size_ = N * sizeof(T);
            h_map_file_ = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, size_, name_.c_str());
            if (h_map_file_ == nullptr) {
                ERR(L"Could not create file mapping object.");
                //throw std::runtime_error("Could not create file mapping object.");
            }
            p_buf_ = static_cast<T*>(MapViewOfFile(h_map_file_, FILE_MAP_ALL_ACCESS, 0, 0, size_));
            if (p_buf_ == nullptr) {
                CloseHandle(h_map_file_);
                ERR(L"Could not map view of file.");
                //throw std::runtime_error("Could not map view of file.");
            }
        }

        ~shared_memory_array() {
            UnmapViewOfFile(p_buf_);
            CloseHandle(h_map_file_);
        }

        bool write(const T(&data)[N])
        {
            //  std::lock_guard<std::mutex> lock(mutex_);
            memcpy(p_buf_, data, N * sizeof(T));
            return true;
        }

        bool read(T(&data)[N])
        {
            // std::lock_guard<std::mutex> lock(mutex_);
            memcpy(data, p_buf_, N * sizeof(T));
            return true;
        }
    };



    template <typename T>
    class shared_memory_single {
    private:
        std::wstring name_;
        size_t size_;
        HANDLE h_map_file_;
        T* p_buf_;
        std::mutex mutex_;

    public:
        shared_memory_single(const std::wstring& name) : name_(name), size_(sizeof(T))
        {
            h_map_file_ = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, size_, name.c_str());
            if (h_map_file_ == nullptr) {
                ERR(L"Could not create file mapping object.");
                //throw std::runtime_error("Could not create file mapping object.");
            }
            p_buf_ = static_cast<T*>(MapViewOfFile(h_map_file_, FILE_MAP_ALL_ACCESS, 0, 0, size_));
            if (p_buf_ == nullptr) {
                CloseHandle(h_map_file_);
                ERR(L"Could not map view of file.");
                //throw std::runtime_error("Could not map view of file.");
            }
        }
        ~shared_memory_single() {
            UnmapViewOfFile(p_buf_);
            CloseHandle(h_map_file_);
        }

        bool write(T* v)
        {
            size_t sz = sizeof(T);
            if (sz > size_)
            {
                throw std::runtime_error("Struct too big dude.");
            }
            memcpy(p_buf_, v, sz);
            return true;
        }

        bool read(T* v)
        {
            size_t sz = sizeof(T);
            memcpy(v, p_buf_, sz);
            return true;
        }
    };

}

template <typename T, size_t MAX_SZ>
class shm_arr
{
    _internal_shm::shared_memory_array<T, MAX_SZ>* shm_;
    T arr_[MAX_SZ];
    const size_t sz_ = MAX_SZ;

public:
    explicit shm_arr(std::wstring name)
    {
        shm_ = new _internal_shm::shared_memory_array < T, MAX_SZ>(name);
    }

    void clear()
    {
        for (auto& i : arr_)
        {
            i = {};
        }
    }

    void clear_from(size_t index)
    {
        if (index >= sz_)
        {
            throw std::runtime_error("Index out of bounds (shm_arr.operator[])");
        }
        for (; index < sz_; ++index)
        {
            arr_[index] = {};
        }
    }

    void write()
    {
        shm_->write(arr_);
    }


    void read()
    {
        shm_->read(arr_);
    }

    T* operator[](size_t i)
    {
        if (i >= sz_)
        {
            throw std::runtime_error("Index out of bounds (shm_arr.operator[])");
        }
        return &arr_[i];
    }

    size_t size() const
    {
        return sz_;
    }

    using iterator = T*;
    using const_iterator = const T*;

    iterator begin() {
        return arr_;
    }

    iterator end() {
        return arr_ + sz_;
    }

    const_iterator begin() const {
        return arr_;
    }

    const_iterator end() const {
        return arr_ + sz_;
    }

    const_iterator cbegin() const {
        return arr_;
    }

    const_iterator cend() const {
        return arr_ + sz_;
    }

};
