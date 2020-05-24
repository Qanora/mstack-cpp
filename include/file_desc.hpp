#pragma once

#include <iostream>
#include <optional>

#include <glog/logging.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace mstack {

class file_desc {
private:
    int _fd;
    file_desc(int fd)
        : _fd(fd)
    {
    }

public:
    static constexpr int RDWR = O_RDWR;
    static constexpr int NONBLOCK = O_NONBLOCK;

public:
    file_desc(): _fd(-1){};
    ~file_desc()
    {
        if (_fd != -1)
            ::close(_fd);
    }

    file_desc(const file_desc&) = default;
    file_desc(file_desc&& other)
        : _fd(other._fd)
    {
        other._fd = -1;
    }

    file_desc& operator=(const file_desc&) = default;
    file_desc& operator=(file_desc&& other)
    {
        if (this != &other) {
            std::swap(_fd, other._fd);
        }
        return *this;
    }
    operator bool() {
        return _fd != -1;
    }
public:
    static std::optional<file_desc> from_fd(int fd)
    {
        if (fd == -1)
            return std::nullopt;
        return file_desc(fd);
    }
    static std::optional<file_desc> open(std::string name, int flags)
    {
        int fd = ::open(name.c_str(), flags);
        if (fd == -1) {
            DLOG(ERROR) << "[OPEN FAIL] " << name << " " << strerror(errno);
        }
        return from_fd(fd);
    }

public:
    int get_fd() { return _fd; }
    template <class X>
    int ioctl(int request, X& data)
    {
        int err = ::ioctl(_fd, request, &data);
        if (err < 0) {
            DLOG(ERROR) << "[IOCTL FAIL] " << strerror(errno);
        }
        return err;
    }

    template <class X>
    int ioctl(int request, X&& data)
    {
        return ioctl(_fd, request, data);
    }
};
}; // namespace mstack