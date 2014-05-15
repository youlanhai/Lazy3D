#pragma once

namespace Lazy
{

//////////////////////////////////////////////////////////////////////////
//文件搜索
//////////////////////////////////////////////////////////////////////////
class cFindFile
{
public:
	cFindFile() : m_handle(NULL) {   }
	~cFindFile(){ reset(); }

	//执行第一次查找
	bool findFirst(const tstring & name);

	//后续查找。必须调用一次findFirst才可用。
	bool findNext();

	bool valid() const { return NULL != m_handle; }

    const tstring & fileName() const { return m_fname; }

	bool isDot() const
	{ 
		const tstring & fname = fileName();
		return fname == _T(".") || fname==_T(".."); 
	}

	bool isDir() const
	{
		return (m_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}

private:

	void reset();

private:
    tstring         m_fname;
	WIN32_FIND_DATA m_data;
	HANDLE          m_handle;
};

}//namespace Lazy