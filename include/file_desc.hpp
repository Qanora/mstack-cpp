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
    file_desc() = delete;
    ~file_desc()
    {
        if (_fd != -1)
            ::close(_fd);
    }

    file_desc(const file_desc&) = delete;
    file_desc(file_desc&& x)
        : _fd(x._fd)
    {
        x._fd = -1;
    }

    file_desc& operator=(const file_desc&) = delete;
    file_desc& operator=(file_desc&& x)
    {
        if (this != &x) {
            auto tmp = file_desc(x._fd);
            std::swap(_fd, tmp._fd);
        }
        return *this;
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
            LOG(ERROR) << "[OPEN FAIL] " << name << " " << strerror(errno);
        }
        return from_fd(fd);
    }

public:
    template <class X>
    int ioctl(int request, X& data)
    {
        int err = ::ioctl(_fd, request, &data);
        if (err < 0) {
            LOG(ERROR) << "[IOCTL FAIL] " << strerror(errno);
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