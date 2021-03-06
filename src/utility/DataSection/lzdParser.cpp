﻿//////////////////////////////////////////////////////////////////////////
/*
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "lzdParser.h"
#include "lzd.h"

namespace Lazy
{

#define PARSE_TEST

#ifdef PARSE_TEST
#include <iostream>
    void testOutput(tstring str)
    {
        //std::cout<<str<<std::endl;
        OutputDebugString(str.c_str());
    }
#endif

    namespace
    {
        const tstring WhiteSpace = _T(" \t\r\n");//空白符

        const tstring WhiteSpaceNoRetrun = _T(" \t\r");//空白符，不包括换行符
        const tstring EndString = _T("}#;\n");//结束符
        const tstring TransferedString = _T("{}#;\"\\");//转义字符
    }

    bool isWhiteSpace(tchar ch)
    {
        return WhiteSpace.find(ch) != WhiteSpace.npos;
    }

    bool isVarNameValid(const tstring & name)
    {
        return true;
    }

    size_t findCh(const char *buffer, size_t n, char ch)
    {
        for (size_t i = 0; i < n; ++i)
        {
            if (buffer[i] == ch) return i;
        }
        return n;
    }

    ///解析编码头
    /*static*/ size_t LzdParser::parseCodingHeader(const char *buffer, size_t len)
    {
        //找出第一行
        len = findCh(buffer, len, '\n');

        //找出#号
        size_t head = findCh(buffer, len, '#') + 1;
        if (head >= len) return 0;

        //找出等号
        size_t eq = findCh(buffer, len, '=');
        if (eq >= len) return 0;

        std::string name(buffer + head, buffer + eq);
        trimStringA(name);
        if (name != "coding") return 0;

        size_t n = len - eq - 1;

        tstring code(n, 0);
        for (size_t i = 0; i < n; ++i)
        {
            code[i] = (tchar) buffer[eq + i + 1];
        }

        trimString(code);
        return codeToCp(code);
    }

    ///字符转义
    /*static*/ void LzdParser::real2transString(tstring & dest, const tstring & src)
    {
        if (src.empty())
            return;

        dest.reserve(src.size());
        for (size_t i = 0; i < src.size(); ++i)
        {
            tchar ch = real2transChar(src[i]);

            if (ch)
            {
                dest += _T('\\');
                dest += ch;
            }
            else
            {
                dest += src[i];
            }
        }
    }

    /*static*/ void LzdParser::trans2realString(tstring & dest, const tstring & src)
    {
        if (src.empty()) return;
        
        dest.reserve(src.size());

        size_t n = src.size() - 1;
        for (size_t i = 0; i < n; ++i)
        {
            tchar ch = src[i];
            if (ch == _T('\\'))
            {
                tchar chNext = src[i + 1];
                ch = trans2realChar(chNext);
                if (ch)
                {
                    dest += ch;
                    ++i;
                }
                else
                {
                    //无效的转义，忽略‘\’符号
                    debugMessage(_T("WARNING: In trans2realString, found invalid '\\' in pos %u of str '%s'"),
                        i, src.c_str());
                }
            }
            else
            {
                dest += ch;
            }
        }

        if (src[n] != _T('\\')) dest += src[n];
    }

    ///字符转义
    /*static*/ tchar LzdParser::real2transChar(tchar ch)
    {
        if (ch == _T('\n'))
        {
            return _T('n');
        }
        else if (ch == _T('\r'))
        {
            return _T('r');
        }
        else if (ch == _T('\t'))
        {
            return _T('t');
        }
        else if (TransferedString.find(ch) != tstring::npos)
        {
            return ch;
        }
        else
        {
            return 0;//没有转义成功
        }
    }

    /*static*/ tchar LzdParser::trans2realChar(tchar ch)
    {
        if (ch == _T('n'))
        {
            return _T('\n');
        }
        else if (ch == _T('r'))
        {
            return _T('\r');
        }
        else if (ch == _T('t'))
        {
            return _T('\t');
        }
        else if (TransferedString.find(ch) != tstring::npos)
        {
            return ch;
        }
        else
        {
            return 0;//没有转义成功
        }
    }

    //////////////////////////////////////////////////////////////////////////

    LzdParser::LzdParser(StrStreamPtr stream, LZDataPtr curNode, int lineNo)
        : m_stream(stream)
        , m_root(curNode)
        , m_lineNo(lineNo)
        , m_errorNo(0)
        , m_errorLine(0)
        , m_bEnd(false)
    {
    }

    LzdParser::~LzdParser(void)
    {
    }

    void LzdParser::parse(void)
    {
        m_bEnd = false;

        while (!error() && !m_stream->empty() && !m_bEnd)
        {
            tchar ch = m_stream->getchar();

            switch (ch)
            {
            case _T('{'):
                onLBraket();
                break;
            case _T('}'):
                onRBraket();
                break;
            case _T('='):
                onEqual();
                break;
            case _T('\n'):
                onReturn();
                break;
            case _T('#'):
                onComment();
                break;
            case _T(';'):
                break;
            default:
                onOther(ch);
                break;
            }
        }
    }

    void LzdParser::onLBraket(void)
    {
        trimString(m_curName);

        if (!m_curName.empty())
        {
            m_lastNode = m_root->newChild(m_curName);
        }
        else if (!m_cacheName.empty())
        {
            m_lastNode = m_root->newChild(m_cacheName);
        }
        else if (!m_lastNode)
        {
            setError(ErrorCode::InvalidLBraket, m_lineNo);
            return;
        }

        LzdParser parser(m_stream, m_lastNode, m_lineNo);
        parser.parse();

        m_lineNo = parser.getLineNo();
        m_errorNo = parser.getErrorNo();
        m_errorLine = parser.getErrorLine();

        if (error()) return;

        m_curName.clear();
        m_cacheName.clear();
    }

    void LzdParser::onRBraket(void)
    {
        m_bEnd = true;
    }

    void LzdParser::onEqual(void)
    {
        trimString(m_curName);
        if (m_curName.empty())
        {
            setError(ErrorCode::InvalidAssignment, m_lineNo);
            return;
        }

        m_lastNode = m_root->newChild(m_curName, getAStr());

        m_curName.clear();
        m_cacheName.clear();
    }

    void LzdParser::onReturn(void)
    {
        trimString(m_curName);
        if (!m_curName.empty())
        {
            m_cacheName = m_curName;//缓存名称
            m_curName.clear();
        }

        ++m_lineNo;
    }

    ///注释‘#’
    void LzdParser::onComment(void)
    {
        skipToReturn();
    }

    ///分号
    void LzdParser::onSemicolon(void)
    {
        m_curName.clear();
        m_cacheName.clear();
    }

    void LzdParser::onOther(tchar ch)
    {
        m_curName += ch;
        //testOutput(m_curVariable);
    }

    void LzdParser::skipToEnd(void)
    {
        while (!m_stream->empty())
        {
            tchar ch = m_stream->curchar();
            if (ch == _T('\n') || ch == _T('}')) break;

            m_stream->ignore(1);
        }
    }

    ///跳的回车符处
    void LzdParser::skipToReturn(void)
    {
        while (!m_stream->empty())
        {
            tchar ch = m_stream->curchar();
            if (ch == _T('\n')) break;

            m_stream->ignore(1);
        }
    }

    /** 从当前位置起，分析出一个合法的字符串（即遇到结束符[}，#，\n，；]位置 ）。
        */
    tstring LzdParser::getAStr(void)
    {
        tstring str;
        tchar ch;

        while (!m_stream->empty())
        {
            ch = m_stream->curchar();

            if (ch == _T('\\'))//转义符
            {
                str += m_stream->getchar();
                str += m_stream->getchar();
            }
            else if (EndString.find(ch) != EndString.npos) //结束符
            {
                break;
            }
            else
            {
                str += m_stream->getchar();
            }
        }

        trimString(str);

        tstring dest;
        trans2realString(dest, str);
        return dest;
    }

}//namespace Lazy