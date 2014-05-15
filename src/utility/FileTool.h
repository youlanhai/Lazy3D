//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#ifndef LAZY_UTILITY_FILETOOL_H
#define LAZY_UTILITY_FILETOOL_H

#include "SmartPtr.h"

namespace Lazy
{
//seek文件长度
long fseeklength(FILE *pFile);

//////////////////////////////////////////////////

//格式化斜杠字符
void formatSlash(tstring & name);

//格式化文件夹中的斜线。尾部追加斜线。
void formatPathName(tstring & dirName, bool appendSlash = true);

#define formatDirName formatPathName


//获得路径中的文件名。即去除路径名，与getFilePath相反。
tstring getPathFile(const tstring & filename);

//获得文件名中的路径。即去除文件名，与getPathFile相反。
tstring getFilePath(const tstring & filename);

//获得文件扩展名。不含'.'
tstring getFileExt(const tstring & filename);

//去除文件名路径
void removeFilePath(tstring & filename);

//去除扩展名。
void removeFileExt(tstring & filename);

//////////////////////////////////////////////////
//File System
//////////////////////////////////////////////////

typedef RefPtr<class FileSystem> FileSystemPtr;

FileSystemPtr getfs();
void setfs(FileSystemPtr fs);


///文件系统
class FileSystem : public IBase
{
public:
    FileSystem();
    virtual ~FileSystem();

    ///缺省路径
    const tstring & defaultPath() const { return m_defaultPath; }
    void setDefaultPath(tstring & path);

    ///添加资源路径
    void addResPath(const tstring & path);
    
    ///递归建立文件夹
    bool makeDirDeep(const tstring & path);

    //interface
    virtual bool getRealPath(tstring & real, const tstring & src) = 0;
    virtual bool searchFile(tstring & realPath, const tstring & fname, const tstring & defaultPath) = 0;

    virtual FILE* openFile(const tstring & fname, const tstring & mode) = 0;

    virtual bool readString(const tstring & fname, std::string & content) = 0;
    virtual bool readBinary(const tstring & fname, std::vector<char> & content) = 0;

    virtual bool writeString(const tstring & fname, const std::string & content) = 0;
    virtual bool writeBinary(const tstring & fname, const void* pData, size_t length) = 0;

    //文件或文件夹是否存在
    virtual bool fileExist(const tstring & name) = 0;

    //是否是文件
    virtual bool isFile(const tstring & name) = 0;

    //是否是文件夹
    virtual bool isDir(const tstring & name) = 0;

    //删除文件
    virtual bool removeFile(const tstring & name) = 0;

    //新建文件夹
    virtual bool makeDir(const tstring & dir) = 0;

    //列举文件夹
    virtual void listDir(StringPool & vect, const tstring & dir) = 0;

    //获得当前工作路径
    virtual tstring getcwd() = 0;

    //设置当前工作路径
    virtual bool setcwd(const tstring & path) = 0;

protected:
    tstring     m_defaultPath;
    StringPool  m_paths;
};

//the default file system
class DefFileSystem : public FileSystem
{
public:
    //读写文件相关
    bool getRealPath(tstring & real, const tstring & src);

    bool searchFile(tstring & realPath, const tstring & fname, const tstring & defaultPath);

    FILE* openFile(const tstring & fname, const tstring & mode);

    bool readString(const tstring & fname, std::string & content);

    bool readBinary(const tstring & fname, std::vector<char> & content);

    bool writeString(const tstring & fname, const std::string & content);

    bool writeBinary(const tstring & fname, const void* pData, size_t length);
   
public:
    //文件或文件夹是否存在
    bool fileExist(const tstring & name);

    //是否是文件
    bool isFile(const tstring & name);

    //是否是文件夹
    bool isDir(const tstring & name);

    //删除文件
    bool removeFile(const tstring & name);

    //新建文件夹
    bool makeDir(const tstring & dir);

    //列举文件夹
    void listDir(StringPool & vect, const tstring & dir);

    //获得当前工作路径
    tstring getcwd();

    //设置当前工作路径
    bool setcwd(const tstring & path);
};


class FileHoder
{
    FILE *m_pFile;

    MAKE_UNCOPY(FileHoder);

public:
    FileHoder(FILE *pFile)
        : m_pFile(pFile)
    {

    }

    ~FileHoder()
    {
        if (m_pFile)
        {
            fclose(m_pFile);
            m_pFile = nullptr;
        }
    }

    operator bool() const
    {
        return m_pFile != nullptr;
    }

    inline FILE* ptr(){ return m_pFile; }
};

}//namespace Lazy

#endif //LAZY_UTILITY_FILETOOL_H