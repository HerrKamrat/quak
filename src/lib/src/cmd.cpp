#include "cmd.hpp"

static const char* CMD_FLAG_EVAL = "--eval";
static const char* CMD_FLAG_EVAL_SHORT = "-e";

static const char* CMD_FLAG_DIR = "--dir";
static const char* CMD_FLAG_DIR_SHORT = "-d";

CmdArgs::CmdArgs(int argc, char** argv) : m_argc(argc), m_argv(argv) {
    char* b = m_argv[0];
    int l = strlen(b);
    char* e = b + l;
    char* c = e;
    while ((--c) > b && *c != '\\') {
    }

    m_cwd.append(b, c);
    m_exec.append(c + 1, e);
}

const char* CmdArgs::exec() {
    return m_exec.data();
}

const char* CmdArgs::cwd() {
    return m_cwd.data();
}

const char* CmdArgs::dir(const char* defaultValue) {
    const char* v = get(CMD_FLAG_DIR, CMD_FLAG_DIR_SHORT);
    if (!v && m_argc > 1) {
        v = value(m_argc - 1);
    }
    return v ? v : defaultValue;
}

const char* CmdArgs::script(const char* defaultValue) {
    const char* v = get(CMD_FLAG_EVAL, CMD_FLAG_EVAL_SHORT);
    if (!v && m_argc > 1) {
        v = value(m_argc - 1);
    }
    return v ? v : defaultValue;
}

bool CmdArgs::has(const char* longFlag, const char* shortFlag) {
    return flagIndexOf(longFlag, shortFlag) > 0;
}

const char* CmdArgs::get(const char* longFlag, const char* shortFlag) {
    int i = flagIndexOf(longFlag, shortFlag);
    return i <= 0 ? nullptr : value(i + 1);
}
bool CmdArgs::empty() {
    return m_argc <= 1;
}
const char* CmdArgs::value(int i) {
    return i >= 0 && i < m_argc && m_argv[i][0] != '-' ? m_argv[i] : nullptr;
}

int CmdArgs::flagIndexOf(const char* longFlag, const char* shortFlag) {
    int i0 = longFlag == nullptr ? -1 : indexOf(longFlag);
    int i1 = shortFlag == nullptr ? -1 : indexOf(shortFlag);
    return i0 > i1 ? i0 : i1;
}

int CmdArgs::indexOf(const char* c) {
    for (int i = 1; i < m_argc; i++) {
        const char* a = m_argv[i];
        if (strcmp(a, c) == 0) {
            return i;
        }
    }
    return -1;
}
