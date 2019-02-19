#ifndef FILE_HPP
#define FILE_HPP

#include <string>
#include <string_view>
#include <vector>

#include "buffer.hpp"


//class directory_entry;
//class directory_iterator;

class FileSystem
{
  public:
    enum class standard_path
    {
        app_data
    };

    enum class file_type
    {

        none,
        not_found,
        regular,
        directory,
        symlink,
        //block,
        //character,
        //fifo,
        //socket,
        unknown,
    };

    class file_status {
        public:
            const file_type type;
            const std::uintmax_t size;
            const std::intmax_t modtime;
            const std::intmax_t createtime;
            const std::intmax_t accesstime;
            const bool readonly;
    };

    
    //   Member types
    using path = std::string_view;
    using error_code = int;
    class filesystem_error : public std::runtime_error {
        public:
            using std::runtime_error::runtime_error;
    };

    // Constructors
    FileSystem(const char *cwd = nullptr);

    // Member methods
    std::string getAppDataPath(const std::string& org_name, const std::string& app_name);
    std::string getAppDataPath(const std::string& org_name, const std::string& app_name, error_code &err) noexcept;
    
    bool setWritablePath(const path& url);
    bool setWritablePath(const path& url, error_code &err) noexcept;

    bool mount(const path& url);
    bool mount(const path& url, error_code& err) noexcept;

	bool mount(const path& url, const std::string& mountPoint);
    bool mount(const path& url, const std::string& mountPoint, error_code& err) noexcept;


    bool exists(const path &url);
    bool exists(const path &url, error_code &err) noexcept;

    std::uintmax_t file_size(const path &uri);
    std::uintmax_t file_size(const path &uri, error_code &err) noexcept;

    std::vector<std::string> read_dir(const path& url);
    std::vector<std::string> read_dir(const path& url, error_code &err) noexcept;

    file_status status(const path& url);
    file_status status(const path& url, error_code &err) noexcept;
    
    bool is_directory(const path& url);
    bool is_directory(const path& url, error_code &err) noexcept;

    bool is_regular_file(const path& url);
    bool is_regular_file(const path& url, error_code &err) noexcept;

    std::unique_ptr<Buffer> read(const path& url);
    std::unique_ptr<Buffer> read(const path& url, error_code& err) noexcept;
    
    bool write(const path& url, const Buffer& buffer);
    bool write(const path& url, const Buffer& buffer, error_code& err) noexcept;

    bool write(const path& url, const std::string_view& buffer);
    bool write(const path& url, const std::string_view& buffer, error_code& err) noexcept;

	std::string normalize_path(const path& url);
    std::string normalize_path(const path& url, error_code& err) noexcept;

  protected:
  private:
    
};

#endif
