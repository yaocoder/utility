/**
* @file path_utils.h
*/
#ifndef SECPLATFORM_COMMON_PATHUTILS_H
#define SECPLATFORM_COMMON_PATHUTILS_H

#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Psapi.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Psapi.lib ")

namespace path_utils
{

/**
 * 获得Windows安装目录：%windir%
 */
static std::string GetWindowsDir()
{
    std::string win_dir;

    unsigned long size = 1024;
    std::string tmp;
    tmp.resize(size);

    unsigned int ret = GetWindowsDirectoryA(&tmp[0], size);
    if (ret > size)
    {
        size = ret;
        tmp.resize(size);
        ret = GetWindowsDirectoryA(&tmp[0], size);
    }
    if (0 == ret)
    {
        throw (std::runtime_error("Failed to get system directory."));
    }
    win_dir.assign(tmp.data(), ret);
    return win_dir;
}

/**
 * 获得系统目录：%system%
 */
static std::string GetSystemDir()
{
    std::string sys_dir;

    unsigned long size = 1024;
    std::string tmp;
    tmp.resize(size);

    unsigned int ret = GetSystemDirectoryA(&tmp[0], size);
    if (ret > size)
    {
        size = ret;
        tmp.resize(size);
        ret = GetSystemDirectoryA(&tmp[0], size);
    }
    if (0 == ret)
    {
        throw (std::runtime_error("Failed to get system directory."));
    }
    sys_dir.assign(tmp.data(), ret);
    return sys_dir;
}

/**
 * 获得当前模块文件的完整路径
 */
static std::string GetModuleFullPath(HMODULE module)
{
    std::string module_full_path;

    unsigned long size = 1024;
    std::string tmp;
    tmp.resize(size);
    unsigned long ret = GetModuleFileNameExA(GetCurrentProcess(), module, &tmp[0], size);
    while (ret == (size-1))
    {
        size *= 2;
        tmp.resize(size);
        ret = GetModuleFileNameExA(GetCurrentProcess(), NULL, &tmp[0], size);
    }
    if (0 == ret)
    {
        throw (std::runtime_error("Failed to get module directory."));
    }

    module_full_path.assign(tmp.data(), ret);

    return module_full_path;
}

/**
 * 获得程序文件的完整路径
 */
static const std::string& GetAppFullPath()
{
    static std::string app_full_path;

    if (app_full_path.empty())
    {
        app_full_path = GetModuleFullPath(NULL);
    }

    return app_full_path;
}

/**
 * 获得模块文件所在目录
 */
static std::string GetModuleDirectory(HMODULE module)
{
    std::string module_dir;

    std::string tmp;
    tmp = GetModuleFullPath(module);
    PathRemoveFileSpecA(&tmp[0]);
    module_dir = tmp.substr(0, tmp.find('\0'));

    return module_dir;
}

/**
 * 获得程序文件所在目录
 */
static const std::string& GetAppDirectory()
{
    static std::string app_dir;

    if (app_dir.empty())
    {
        app_dir = GetModuleDirectory(NULL);
    }

    return app_dir;
}

/**
 * 重置当前目录为进程所在目录
 */
static void ResetCurrentDirectory()
{
    SetCurrentDirectoryA(GetAppDirectory().c_str());
}

}

#endif // SECPLATFORM_COMMON_PATHUTILS_H