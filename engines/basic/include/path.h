#ifndef DELTA_BASIC_PATH_H
#define DELTA_BASIC_PATH_H

#include <string>

#if USE_CXX17_FILESYSTEM
#include <filesystem>
#else // USE_CXX17_FILESYSTEM
namespace boost {
    namespace filesystem {
        class path;
    } /* namespace filesystem */
} /* namespace boost */
#endif // USE_CXX17_FILESYSTEM

namespace dbasic {

#if USE_CXX17_FILESYSTEM
    namespace filesystem = std::filesystem;
#else // USE_CXX17_FILESYSTEM
    namespace filesystem = boost::filesystem;
#endif // USE_CXX17_FILESYSTEM

    class Path {
    protected: Path(const filesystem::path &path);
    public:
        Path(const std::wstring &path);
        Path(const wchar_t *path);
        Path(const Path &path);
        Path();
        ~Path();

        std::wstring ToString() const;

        void SetPath(const std::wstring &path);
        bool operator==(const Path &path) const;
        Path Append(const Path &path) const;

        void GetParentPath(Path *path) const;

        const Path &operator=(const Path &b);

        std::wstring GetExtension() const;
        std::wstring GetStem() const;

        Path GetAbsolute() const;

        bool IsAbsolute() const;
        bool Exists() const;

    protected:
        filesystem::path *m_path;

    protected:
        const filesystem::path &GetPath() const { return *m_path; }
    };

} /* namespace dbasic */

#endif /* DELTA_BASIC_PATH_H */
