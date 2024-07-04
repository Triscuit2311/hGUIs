// shared_memory_vec.h
#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <mutex>
#include <stdexcept>
#include <algorithm>
#include <array>




template <typename T>
class shared_memory_vec {

private:
    std::wstring name_;
    size_t size_;
    HANDLE h_map_file_;
    size_t* num_elements;
    T* p_buf_;
    std::mutex mutex_;

public:
    shared_memory_vec(const std::wstring& name, const size_t max_elements) : name_(name), size_((max_elements*sizeof(T)))
    {
        h_map_file_ = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size_, name.c_str());
        if (h_map_file_ == nullptr) {
            throw std::runtime_error("Could not create file mapping object.");
        }
        p_buf_ = static_cast<T*>(MapViewOfFile(h_map_file_, FILE_MAP_ALL_ACCESS, 0, 0, size_));
        if (p_buf_ == nullptr) {
            CloseHandle(h_map_file_);
            throw std::runtime_error("Could not map view of file.");
        }
    }
    ~shared_memory_vec() {
        UnmapViewOfFile(p_buf_);
        CloseHandle(h_map_file_);
    }

    bool write(std::vector<T> data)
    {
        size_t sz = data.size() * sizeof(T);
	    if(sz > size_)
	    {
            throw std::runtime_error("Array too big dude.");
	    }
        memcpy(p_buf_, data.data(), sz);
        return true;
    }

    bool read(std::vector<T>& data)
    {
        size_t sz = data.size() * sizeof(T);
        memcpy(data.data(), p_buf_, sz);
        return true;
    }
};



template <typename T, size_t N>
class shared_memory_array {
private:
    std::wstring name_;
    HANDLE h_map_file_;
    T* p_buf_;
    std::mutex mutex_;

public:
    shared_memory_array(const std::wstring& name) : name_(name)
    {
        size_t size_ = N * sizeof(T);
        h_map_file_ = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size_, name_.c_str());
        if (h_map_file_ == nullptr) {
            throw std::runtime_error("Could not create file mapping object.");
        }
        p_buf_ = static_cast<T*>(MapViewOfFile(h_map_file_, FILE_MAP_ALL_ACCESS, 0, 0, size_));
        if (p_buf_ == nullptr) {
            CloseHandle(h_map_file_);
            throw std::runtime_error("Could not map view of file.");
        }
    }

    ~shared_memory_array() {
        UnmapViewOfFile(p_buf_);
        CloseHandle(h_map_file_);
    }

    bool write(const T(&data)[N])
    {
        std::lock_guard<std::mutex> lock(mutex_);
        memcpy(p_buf_, data, N * sizeof(T));
        return true;
    }

    bool read(T(&data)[N])
    {
        std::lock_guard<std::mutex> lock(mutex_);
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
            throw std::runtime_error("Could not create file mapping object.");
        }
        p_buf_ = static_cast<T*>(MapViewOfFile(h_map_file_, FILE_MAP_ALL_ACCESS, 0, 0, size_));
        if (p_buf_ == nullptr) {
            CloseHandle(h_map_file_);
            throw std::runtime_error("Could not map view of file.");
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