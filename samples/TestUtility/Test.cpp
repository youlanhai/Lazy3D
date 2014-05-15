// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "utility/Utility.h"

#include <cassert>
#include <iostream>
#include <sstream>
using namespace std;

#ifdef _UNICODE
#	define TCout wcout
#else
#	define TCout cout
#endif

void testCase(bool ret, const char * name = "")
{
    if (ret)
    {
        std::cout << "succed: " << name << std::endl;
    }
    else
    {
        std::cout << "failed: " << name << std::endl;
        exit(1);
    }
}

void testTrim(Lazy::tstring str, const Lazy::tstring & o)
{
    TCout<<_T("src str:'")<<str;
    trimString(str);
    TCout<<_T("' trim:'")<<str<<_T("'")<<endl;

    assert(str == o);
}

void doTest();

int _tmain(int argc, _TCHAR* argv [])
{
    Lazy::defaultCodePage = CP_UTF8;

    Lazy::MemoryPool::init();

    try
    {
        doTest();
    }
    catch (std::exception e)
    {
        std::cout << "found exception: " << e.what() << std::endl;
    }

    Lazy::clearSectionCache();

    Lazy::MemoryPool::fini();
    Lazy::MemoryCheck::instance()->release();

    return 0;
}

void doTest()
{
    Lazy::tstring strTest;

#if 0
    {//测试内存池

        Lazy::tstring memoryInfo;
        Lazy::MemoryPool * pool = Lazy::MemoryPool::instance();

        void * p = pool->alloc(3);
        pool->free(p);

        size_t n = 10;
        while (n--)
        {

            const size_t nCase = 256;
            std::vector<void*> mem;
            for (size_t i = 0; i < nCase; ++i)
                mem.push_back(pool->alloc(i));

            pool->dump(memoryInfo);
            TCout << memoryInfo << std::endl;

            for (size_t i = 0; i < nCase; ++i)
                pool->free(mem[i]);

        }

        pool->dump(memoryInfo);
        TCout << memoryInfo << std::endl;

        pool->alloc(255);
        pool->alloc(256);
        pool->alloc(257);

        pool->dump(memoryInfo);
        TCout << memoryInfo << std::endl;
    }
#endif

#if 1
    {
        //test log
        INIT_LOG(_T("test.log"));

        WRITE_LOG(L"测试中文log。");
        XWRITE_LOG(_T("测试log:%d, %f, %s"), 12, 3.4, _T("哈哈,abc"));

        LOG_WRITE(L"测试LOG_WRITE");
        LOG_DEBUG(L"测试LOG_DEBUG");
        LOG_INFO(L"测试LOG_INFO");
        LOG_WARNING(L"测试LOG_WARNING");
        LOG_ERROR(L"测试LOG_ERROR");
    }
#endif


#if 1
    //test guid
	TCout<<_T("generateGUID:")<<Lazy::generateGUID()<<endl;
#endif

#if 1
    {
        //test trim
        strTest = _T(" \t hello world \r\n ");
        TCout << _T("strTest before trim: '") << strTest << _T("'") << endl;

        trimString(strTest);
        TCout << _T("strTest after trim: '") << strTest << _T("'") << endl;

        Lazy::eraseToEnd(strTest, _T('o'));
        TCout << _T("eraseToEnd 'o':") << strTest << endl;

        testTrim(_T(""), _T(""));
        testTrim(_T("x"), _T("x"));
        testTrim(_T("x "), _T("x"));
        testTrim(_T(" x"), _T("x"));
        testTrim(_T(" x "), _T("x"));
        testTrim(_T("\t\r\nx\n"), _T("x"));
    }
#endif

#if 1
    {
        //test format string
        formatString(strTest, _T("formatString:%d, %0.2f, %s"), 12, 3.4, _T("hello,world!"));
        TCout << strTest << endl;

        Lazy::StringConvTool convTool;
        convTool.formatFrom(_T("formatFrom: %d, %0.2f, %s"), 12, 3.4, _T("hello,world!"));
        TCout << convTool.asString() << endl;

        Lazy::buildStringNum(strTest, 123456789, 10);
        testCase(strTest == _T("123456789"), "buildStringNum 10");

        Lazy::buildStringNum(strTest, 0x7fffffff, 2);
        testCase(strTest == _T("1111111111111111111111111111111"), "buildStringNum 2");

        Lazy::buildStringNum(strTest, 0x7fffffff, 16);
        testCase(strTest == _T("7FFFFFFF"), "buildStringNum 16");

        Lazy::buildStringNum(strTest, 0x7fffffff, 8);
        TCout << "buildStringNum 8: " << strTest << std::endl;

        Lazy::buildStringNum(strTest, 0x7fffffff, 5);
        TCout << "buildStringNum 5: " << strTest << std::endl;

        Lazy::buildStringNum(strTest, 0x7fffffff, 24);
        TCout << "buildStringNum 5: " << strTest << std::endl;
    }
#endif

#if 1
    {
        TCout << endl;

        //test format file name
        Lazy::tstring path = _T("F:\\workspace\\utility\\Test.cpp");
        TCout << _T("\ntest format file name") << std::endl;
        TCout << _T("orignal path:") << path << std::endl;

        Lazy::formatSlash(path);
        TCout << _T("formatSlash:") << path << std::endl;

        Lazy::tstring dir = Lazy::getFilePath(path);
        TCout << _T("getFilePath:") << dir << std::endl;
        TCout << _T("    getFilePath:") << Lazy::getFilePath(dir) << std::endl;

        Lazy::formatDirName(dir);
        TCout << _T("    fomatDirName:") << dir << endl;

        TCout << _T("getPathFile:") << Lazy::getPathFile(path) << endl;
        TCout << _T("getFileExt:") << Lazy::getFileExt(path) << endl;

        Lazy::tstring temp(path);
        Lazy::removeFilePath(temp);
        TCout << _T("removeFilePath:") << temp << endl;

        temp = path;
        Lazy::removeFileExt(temp);
        TCout << _T("removeFileExt:") << temp << endl;
    }
#endif

#if 1
    {
        //test file system
        Lazy::getfs()->addResPath(_T("res"));

        Lazy::tstring fname = _T("data/test.txt");
        std::string content = "hello world中文啊abcd.";

        Lazy::getfs()->writeString(fname, content);
        content.clear();

        //fname += _T("hh");
        if (!Lazy::getfs()->readString(fname, content))
        {
            TCout << _T("open file failed!") << fname << endl;
        }
        else
        {
            cout << "read from file:" << content << endl;
        }
    }
#endif


#if 1
    {
        //test data section
        Lazy::LZDataPtr root = Lazy::openSection(_T("test.lzd"));
        if (root)
        {
            root->print(wcout);
            if (!Lazy::saveSection(root, _T("test2.lzd")))
            {
                cout << "save data failed!" << endl;
            }

            Lazy::tstring title = root->readString(_T("title"));
            SetConsoleTitle(title.c_str());
        }
        else
        {
            cout << "open lzd file failed!" << endl;
        }

        root = NULL;//force delete
    }

#endif

}

