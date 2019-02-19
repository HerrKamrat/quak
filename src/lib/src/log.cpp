#include "log.hpp"

#include <SDL_log.h>

void log(const char* msg) {
    SDL_Log("%s", msg);
};

void log(log_level level, const char* msg){
    SDL_LogPriority p = static_cast<SDL_LogPriority>((int)SDL_LOG_PRIORITY_DEBUG + (int)level);
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, p, "%s", msg);
};
