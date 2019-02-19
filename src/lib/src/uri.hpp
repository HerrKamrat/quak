#pragma once

#include <string_view>

class UriView {
    public:

    UriView(const std::string_view& uri);
    

    std::string_view scheme;
    std::string_view userName;
    std::string_view password;
    std::string_view host;
    std::string_view port;
    std::string_view path;
    std::string_view query;
    std::string_view fragment;
};
