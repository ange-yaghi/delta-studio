#include "../include/path.h"

#if !USE_CXX17_FILESYSTEM
#include <boost/filesystem.hpp>
#endif // USE_CXX17_FILESYSTEM

dbasic::Path::Path(const std::string &path) {
    m_path = nullptr;
    SetPath(path);
}

dbasic::Path::Path(const char *path) {
    m_path = nullptr;
    SetPath(std::string(path));
}

dbasic::Path::Path(const Path &path) {
    m_path = new filesystem::path;
    *m_path = path.GetPath();
}

dbasic::Path::Path() {
    m_path = nullptr;
}

dbasic::Path::Path(const filesystem::path &path) {
    m_path = new filesystem::path;
    *m_path = path;
}

dbasic::Path::~Path() {
    delete m_path;
}

std::string dbasic::Path::ToString() const {
    return m_path->string();
}

void dbasic::Path::SetPath(const std::string &path) {
    if (m_path != nullptr) delete m_path;

    m_path = new filesystem::path(path);
}

bool dbasic::Path::operator==(const Path &path) const {
    return filesystem::equivalent(this->GetPath(), path.GetPath());
}

dbasic::Path dbasic::Path::Append(const Path &path) const {
    return Path(GetPath() / path.GetPath());
}

void dbasic::Path::GetParentPath(Path *path) const {
    path->m_path = new filesystem::path;
    *path->m_path = m_path->parent_path();
}

const dbasic::Path &dbasic::Path::operator=(const Path &b) {
    if (m_path != nullptr) delete m_path;

    m_path = new filesystem::path;
    *m_path = b.GetPath();

    return *this;
}

std::string dbasic::Path::GetExtension() const {
    return m_path->extension().string();
}

std::string dbasic::Path::GetStem() const {
    return m_path->stem().string();
}

dbasic::Path dbasic::Path::GetAbsolute() const {
    return filesystem::absolute(*m_path);
}

bool dbasic::Path::IsAbsolute() const {
    return m_path->is_absolute();
}

bool dbasic::Path::Exists() const {
    return filesystem::exists(*m_path);
}
