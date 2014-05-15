#include "StdAfx.h"
#include "FileFind.h"

namespace Lazy
{


bool cFindFile::findFirst(const tstring & name)
{
	reset();

	ZeroMemory(&m_data, sizeof(m_data));
	m_handle = ::FindFirstFile(name.c_str(), &m_data);
	if (INVALID_HANDLE_VALUE == m_handle)
	{
		m_handle = NULL;
	}

    if (m_data.cFileName) m_fname = m_data.cFileName;
    else m_fname.clear();

	return m_handle != NULL;
}

bool cFindFile::findNext()
{
	if (!valid()) return false;

	bool ret = !!::FindNextFile(m_handle, &m_data);

    if (m_data.cFileName) m_fname = m_data.cFileName;
    else m_fname.clear();

    return ret;
}

void cFindFile::reset()
{
	if (valid())
	{
		::FindClose(m_handle);
		m_handle = NULL;
	}
}

}//namespace Lazy

