//////////////////////////////////////////////////////////////////////////
/*
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "FileTool.h"
#include "FileFind.h"
#include "Misc.h"
#include "Log.h"

#include <cassert>

namespace Lazy
{

    inline DWORD fileAttr(const tstring & name)
    {
        return GetFileAttributes(name.c_str());
    }

    void formatSlash(tstring & name)
    {
        std::replace(name.begin(), name.end(), SLASH_OLD, SLASH_USE);
    }

    void formatPathName(tstring & dirName, bool appendSlash/*=true*/)
    {
        if (dirName.empty()) return;

        formatSlash(dirName);

        if (appendSlash)
        {
            if (dirName.back() != SLASH_USE)
                dirName.push_back(SLASH_USE);
        }
        else
        {
            if (dirName.back() == SLASH_USE)
                dirName.pop_back();
        }
    }

    tstring getPathFile(const tstring & filename)
    {
        if (filename.empty()) return filename;

        tstring tempName(filename);
        formatPathName(tempName, false);

        size_t pos = tempName.find_last_of(SLASH_USE);
        if (pos == tempName.npos)
            return tempName;

        return tempName.substr(pos + 1);
    }

//获得文件名中的路径
    tstring getFilePath(const tstring & filename)
    {
        if (filename.empty()) return filename;

        tstring tempName(filename);
        formatPathName(tempName, false);

        size_t pos = tempName.find_last_of(SLASH_USE);
        if (pos == tempName.npos)
            return EmptyStr;

        return tempName.substr(0, pos + 1);
    }

    tstring getFileExt(const tstring & filename)
    {
        tstring tempName(filename);
        size_t pos = tempName.find_last_of(_T('.'));
        if (pos == tempName.npos)
        {
            return EmptyStr;
        }
        return tempName.substr(pos + 1);
    }

    void removeFilePath(tstring & filename)
    {
        if(filename.empty()) return;

        formatPathName(filename, false);

        size_t pos = filename.find_last_of(SLASH_USE);
        if (pos != filename.npos)
        {
            filename.erase(0, pos + 1);
        }
    }

    void removeFileExt(tstring & filename)
    {
        reraseToEnd(filename, _T('.'));
    }
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////


    bool findFiles(StringPool & vect,
                   const tstring & path,
                   const tstring & ext,
                   bool unionPath)
    {

        vect.clear();

        tstring formatedPath = path;
        formatDirName(formatedPath);

        tstring tempName = formatedPath + ext;

        cFindFile finder;
        if(!finder.findFirst(tempName))
        {
            return false;
        }
        do
        {
            if(finder.isDot() || finder.isDir())
            {
                continue;
            }
            if (unionPath)
            {
                vect.push_back(formatedPath + finder.fileName());
            }
            else
            {
                vect.push_back(finder.fileName());
            }
        }
        while(finder.findNext());

        return !vect.empty();
    }


    bool findFile(tstring & realPath, const tstring & fname, const tstring & path, bool resculy)
    {
        tstring formatedPath = path;
        formatDirName(formatedPath);

        tstring tempName = formatedPath + fname;
        if (fileAttr(tempName) != INVALID_FILE_ATTRIBUTES)
        {
            realPath = tempName;
            return true;
        }

        if (!resculy) return false;

        tempName = formatedPath + L"*";

        cFindFile finder;
        if (!finder.findFirst(tempName))
        {
            return false;
        }
        do
        {
            if (finder.isDir() && !finder.isDot())
            {
                if (findFile(realPath, fname, formatedPath + finder.fileName(), resculy)) return true;
            }

        }
        while (finder.findNext());

        return false;
    }

    long fseeklength(FILE *pFile)
    {
        assert(pFile != NULL);

        long cur = ftell(pFile);
        fseek(pFile, 0, SEEK_END);
        long length = ftell(pFile);

        fseek(pFile, cur, SEEK_SET);

        return length;
    }

//////////////////////////////////////////////////
//File System
//////////////////////////////////////////////////

    FileSystem::FileSystem()
    {
    }

    FileSystem::~FileSystem()
    {
    }

    void FileSystem::setDefaultPath(tstring & path)
    {
        m_defaultPath = path;
        formatDirName(m_defaultPath);
    }

    void FileSystem::addResPath(const tstring & path)
    {
        debugMessage(_T("INFO: addResPath '%s'"), path.c_str());

        m_paths.push_back(path);
        formatDirName(m_paths.back());

        if (m_defaultPath.empty())
        {
            m_defaultPath = m_paths.back();
        }
    }

    bool FileSystem::makeDirDeep(const tstring & dir)
    {
        if (fileExist(dir)) return true;

        tstring tempPath = dir;
        formatDirName(tempPath, false);
        tstring parentPath = getFilePath(tempPath);

        if (!parentPath.empty())
        {
            if(!makeDirDeep(parentPath)) return false;
        }

        return makeDir(tempPath);
    }

//default file system

    bool DefFileSystem::getRealPath(tstring & real, const tstring & src)
    {
        if(fileExist(src))
        {
            real = src;
            return true;
        }

        tstring tempName;
        StringPool::const_iterator it;
        FOR_EACH(m_paths, it)
        {
            tempName = (*it) + src;
            if (fileExist(tempName))
            {
                real = tempName;
                return true;
            }
        }

        return false;
    }

    bool DefFileSystem::searchFile(tstring & realPath, const tstring & fname, const tstring & defaultPath)
    {
        if (fileExist(fname))
        {
            realPath = fname;
            return true;
        }

        //外部提供的目录
        if (!defaultPath.empty() && findFile(realPath, fname, defaultPath, true)) return true;

        //当前目录
        if (findFile(realPath, fname, L"./", true)) return true;

        //res目录
        for (tstring & path : m_paths)
        {
            if (findFile(realPath, fname, path, true))
            {
                return true;
            }
        }

        return false;
    }

    FILE* DefFileSystem::openFile(const tstring & fname, const tstring & mode)
    {
        tstring realPath;
        if (mode.find(L'r') != mode.npos)
        {
            if (!getRealPath(realPath, fname))
            {
                LOG_ERROR(L"Can't find file '%s'", fname.c_str());
                return nullptr;
            }
        }
        else
        {
            //优先覆盖已经存在的配置文件
            if (!getRealPath(realPath, fname))
                realPath = defaultPath() + fname;
        }

        FILE* pFile = NULL;
        errno_t error = _wfopen_s(&pFile, realPath.c_str(), mode.c_str());
        if(error != 0)
        {
            LOG_ERROR(L"open file '%s' with '%s' failed!%d", realPath.c_str(), mode.c_str(), error);
            return nullptr;
        }

        return pFile;
    }

    bool DefFileSystem::readString(const tstring & fname, std::string & content)
    {
        FILE* pFile = openFile(fname, _T("r"));
        if(!pFile) return false;

        long length = fseeklength(pFile);
        content.resize(length, 0);

        if(length > 0)
        {
            fread(&content[0], length, 1, pFile);
            eraseToEnd(content, 0);
        }

        fclose(pFile);
        return true;
    }

    bool DefFileSystem::readBinary(const tstring & fname, std::vector<char> & content)
    {
        FILE* pFile = openFile(fname, _T("rb"));
        if(!pFile) return false;

        long length = fseeklength(pFile);
        content.resize(length);

        if(length > 0)
        {
            fread(&content[0], length, 1, pFile);
        }

        fclose(pFile);
        return true;
    }

    bool DefFileSystem::writeString(const tstring & fname, const std::string & content)
    {
        FILE* pFile = openFile(fname, _T("w"));
        if(!pFile) return false;

        fwrite(content.c_str(), content.length(), 1, pFile);

        fclose(pFile);
        return true;
    }

    bool DefFileSystem::writeBinary(const tstring & fname, const void* pData, size_t length)
    {
        FILE* pFile = openFile(fname, _T("wb"));
        if(!pFile) return false;

        fwrite(pData, length, 1, pFile);

        fclose(pFile);
        return true;
    }


//列举文件夹
    void DefFileSystem::listDir(StringPool & vect, const tstring & dir)
    {
        vect.clear();

        tstring formatedPath = dir;
        formatDirName(formatedPath);

        tstring tempName = formatedPath + _T("*");

        cFindFile finder;
        if(!finder.findFirst(tempName)) return;

        do
        {
            if(finder.isDot() || finder.isDir())
            {
                continue;
            }

            vect.push_back(finder.fileName());
        }
        while(finder.findNext());

    }

//获得当前工作路径
    tstring DefFileSystem::getcwd()
    {
        tstring buffer(256, 0);
        ::GetCurrentDirectory(256, &buffer[0]);

        eraseToEnd(buffer, 0);
        return buffer;
    }

    bool DefFileSystem::setcwd(const tstring & path)
    {
        tstring temp;
        if (!path.empty())
        {
            temp = path;
        }
        else
        {
            temp.resize(MAX_PATH, 0);
            GetModuleFileName(NULL, &temp[0], MAX_PATH);
            eraseToEnd(temp, 0);
            temp = getFilePath(temp);
        }
        bool ret = !!SetCurrentDirectory(temp.c_str());

        debugMessage(_T("setcwd: %s"), getcwd().c_str());
        return ret;
    }

//是否是文件
    bool DefFileSystem::isFile(const tstring & name)
    {
        //FILE_ATTRIBUTE_ARCHIVE
        return 0 != (fileAttr(name) & FILE_ATTRIBUTE_NORMAL);
    }

//是否是文件夹
    bool DefFileSystem::isDir(const tstring & name)
    {
        return 0 != (fileAttr(name) & FILE_ATTRIBUTE_DIRECTORY);
    }


    bool DefFileSystem::fileExist(const tstring & name)
    {
        return fileAttr(name) != INVALID_FILE_ATTRIBUTES;
    }

    bool DefFileSystem::removeFile(const tstring & name)
    {
        return !!::DeleteFile(name.c_str());
    }

    bool DefFileSystem::makeDir(const tstring & dir)
    {
        return !!CreateDirectory(dir.c_str(), NULL);
    }

//////////////////////////////////////////////////
    static FileSystemPtr gs_fs = new DefFileSystem();

    FileSystemPtr getfs()
    {
        return gs_fs;
    }

    void setfs(FileSystemPtr fs)
    {
        gs_fs = fs;
    }

}//namespace Lazy
