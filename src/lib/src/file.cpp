#include "file.hpp"

#include <physfs.h>

#include "log.hpp"

inline FileSystem::filesystem_error error(FileSystem::error_code err) {
    auto msg = PHYSFS_getErrorByCode(static_cast<PHYSFS_ErrorCode>(err));
    return FileSystem::filesystem_error(msg);
}

template <typename R>
inline R throw_or_return(R r, FileSystem::error_code& err) {
    if (err) {
        auto msg = PHYSFS_getErrorByCode(static_cast<PHYSFS_ErrorCode>(err));
        throw FileSystem::filesystem_error(msg);
    }
    return r;
}

FileSystem::FileSystem(const char* cwd) {
    if (PHYSFS_init(cwd) == 0) {
        throw error(PHYSFS_getLastErrorCode());
    }
}

std::string FileSystem::getAppDataPath(const std::string& org_name, const std::string& app_name) {
    error_code err;
    return throw_or_return(getAppDataPath(org_name, app_name, err), err);
}

std::string FileSystem::getAppDataPath(const std::string& org_name,
                                       const std::string& app_name,
                                       error_code& err) noexcept {
    err = 0;
    auto res = PHYSFS_getPrefDir(org_name.c_str(), app_name.c_str());
    if (res != nullptr) {
        return res;
    }
    err = PHYSFS_getLastErrorCode();
    return "";
}

bool FileSystem::setWritablePath(const path& url) {
    error_code err;
    return throw_or_return(setWritablePath(url, err), err);
}

bool FileSystem::setWritablePath(const path& url, error_code& err) noexcept {
    err = 0;
    if (PHYSFS_setWriteDir(url.data()) != 0) {
        return mount(url, err);
    }

    err = PHYSFS_getLastErrorCode();
    return false;
}

bool FileSystem::mount(const path& url) {
    error_code err;
    return throw_or_return(mount(url, err), err);
}

bool FileSystem::mount(const path& url, error_code& err) noexcept {
    return mount(url, "/", err);
}

bool FileSystem::mount(const path& url, const std::string& mountPoint) {
    error_code err;
    return throw_or_return(mount(url, mountPoint, err), err);
};

bool FileSystem::mount(const path& url, const std::string& mountPoint, error_code& err) noexcept {
    err = 0;
    if (PHYSFS_mount(url.data(), mountPoint.empty() ? nullptr : mountPoint.c_str(), true) != 0) {
        return true;
    }

    err = PHYSFS_getLastErrorCode();
    return false;
};

bool FileSystem::exists(const path& url) {
    error_code err;
    return throw_or_return(exists(url, err), err);
}

bool FileSystem::exists(const path& url, error_code& err) noexcept {
    err = 0;

    auto path = normalize_path(url, err);
    if (!err) {
        if (PHYSFS_exists(path.data()) != 0) {
            return true;
        }

        auto errCode = PHYSFS_getLastErrorCode();
        err = errCode;
    }

    return false;
}

std::uintmax_t FileSystem::file_size(const path& url) {
    error_code err;
    return throw_or_return(file_size(url, err), err);
}

std::uintmax_t FileSystem::file_size(const path& url, error_code& err) noexcept {
    return status(url, err).size;
}

std::vector<std::string> FileSystem::read_dir(const path& url) {
    error_code err;
    return throw_or_return(read_dir(url, err), err);
}

std::vector<std::string> FileSystem::read_dir(const path& url, error_code& err) noexcept {
    err = 0;
    std::vector<std::string> entries;

    auto path = normalize_path(url, err);
    if (!err) {
        char** e = PHYSFS_enumerateFiles(path.data());
        if (e == nullptr) {
            auto errCode = PHYSFS_getLastErrorCode();
            err = errCode;
        } else {
            for (char** c = e; *c != NULL; c++) {
                entries.push_back(*c);
            }
            PHYSFS_freeList(e);
        }
    }

    return entries;
}

FileSystem::file_status FileSystem::status(const path& url) {
    error_code err;
    return throw_or_return(status(url, err), err);
}

FileSystem::file_status FileSystem::status(const path& url, error_code& err) noexcept {
    static_assert((int)PHYSFS_FILETYPE_REGULAR + 2 == (int)file_type::regular, "");
    static_assert((int)PHYSFS_FILETYPE_DIRECTORY + 2 == (int)file_type::directory, "");
    static_assert((int)PHYSFS_FILETYPE_SYMLINK + 2 == (int)file_type::symlink, "");
    static_assert((int)PHYSFS_FILETYPE_OTHER + 2 == (int)file_type::unknown, "");

    err = 0;

    file_type type = file_type::none;
    PHYSFS_Stat stat = {-1, -1, -1, -1, PHYSFS_FILETYPE_OTHER, 1};

    auto path = normalize_path(url, err);
    if (!err) {
        if (PHYSFS_stat(path.data(), &stat) != 0) {
            type = static_cast<file_type>(stat.filetype + 2);
        } else {
            auto errCode = PHYSFS_getLastErrorCode();
            if (errCode == PHYSFS_ERR_NOT_FOUND) {
                type = file_type::not_found;
            } else {
                err = errCode;
            }
        }
    }

    std::uintmax_t filesize = stat.filesize < 0 ? 0 : stat.filesize;
    return {type, filesize, stat.modtime, stat.createtime, stat.accesstime, stat.readonly != 0};
}

bool FileSystem::is_directory(const path& url) {
    error_code err;
    return throw_or_return(is_directory(url, err), err);
}

bool FileSystem::is_directory(const path& url, error_code& err) noexcept {
    return status(url, err).type == file_type::directory;
}

bool FileSystem::is_regular_file(const path& url) {
    error_code err;
    return throw_or_return(is_regular_file(url, err), err);
}

bool FileSystem::is_regular_file(const path& url, error_code& err) noexcept {
    return status(url, err).type == file_type::regular;
}

std::unique_ptr<Buffer> FileSystem::read(const path& url) {
    error_code err;
    return throw_or_return(read(url, err), err);
}

std::unique_ptr<Buffer> FileSystem::read(const path& url, error_code& err) noexcept {
    err = 0;
    auto path = normalize_path(url, err);
    if (!err) {
        auto file = PHYSFS_openRead(path.data());
        if (file != nullptr) {
            auto buffer = std::make_unique<Buffer>();
            auto size = PHYSFS_fileLength(file);
			//TODO: if size > max(size_t) err
            buffer->resize(static_cast<size_t>(size));
            auto read = PHYSFS_readBytes(file, buffer->data(), size);
            if (read == size) {
                return buffer;
            }

            debug("read: {} / {}", read, size);
        }
        auto errCode = PHYSFS_getLastErrorCode();
        err = errCode;
    }
    return {};
}

bool FileSystem::write(const path& url, const Buffer& data) {
    return write(url, data.view());
}

bool FileSystem::write(const path& url, const Buffer& data, error_code& err) noexcept {
    return write(url, data.view(), err);
}

bool FileSystem::write(const path& url, const std::string_view& data) {
    error_code err;
    return throw_or_return(write(url, data, err), err);
};

bool FileSystem::write(const path& url, const std::string_view& data, error_code& err) noexcept {
    err = 0;
    auto path = normalize_path(url, err);
    if (!err) {
        auto file = PHYSFS_openWrite(path.data());
        if (file != nullptr) {
            auto written = PHYSFS_writeBytes(file, data.data(), data.size());
            if (written == data.size()) {
                return true;
            }

            debug("written: {} / {}", written, data.size());
        }

        auto errCode = PHYSFS_getLastErrorCode();
        err = errCode;
    }

    return false;
};

std::string FileSystem::normalize_path(const path& url) {
    error_code err;
    return throw_or_return(normalize_path(url, err), err);
};

std::string FileSystem::normalize_path(const path& url, error_code& err) noexcept {
    const char DEL = '/';
    error_code error = 0;
    if (url.empty()) {
        err = PHYSFS_ERR_BAD_FILENAME;
        return "";
    }

    std::vector<std::string_view> parts;
    parts.reserve(std::count(url.begin(), url.end(), DEL));

    std::string_view p = url;
    size_t i = 0;
    if (p.front() == DEL) {
        p.remove_prefix(1);
    }

    while ((i = p.find(DEL)) != std::string_view::npos) {
        parts.push_back(p.substr(0, i));
        p = p.substr(i + 1);
    }
    if (!p.empty()) {
        parts.push_back(p);
    }

    size_t length = 0;
    auto it = parts.begin();
    while (it != parts.end()) {
        if (it->empty()) {
            error = PHYSFS_ERR_BAD_FILENAME;
            break;
        } else if (*it == ".") {
            it = parts.erase(it);
        } else if (*it == "..") {
            if (it == parts.begin()) {
                error = PHYSFS_ERR_BAD_FILENAME;
                break;
            }
            it = parts.erase(it - 1, it + 1);
        } else if (it->find("..") != std::string_view::npos) {
            error = PHYSFS_ERR_BAD_FILENAME;
            break;
        } else {
            length += it->size();
            ++it;
        }
    }

    std::string norm;
    if (!error) {
        if (parts.empty()) {
            norm = DEL;
        } else {
            length += parts.size();
            norm.reserve(length);
            for (auto it = parts.begin(); it != parts.end(); ++it) {
                norm += DEL;
                norm.append(*it);
            }
        }
    }
    err = error;
    return norm;
};