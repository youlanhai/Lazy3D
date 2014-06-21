//////////////////////////////////////////////////////////////////////////
/*
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "LZDataBase.h"
#include "StrStream.h"

namespace Lazy
{
    namespace ErrorCode
    {
        const int None = 0;//无错
        const int InvalidName = 1;//无效的名称
        const int InvalidAssignment = 2;//无效赋值
        const int InvalidLBraket = 3;//无效的括号
        const int InvalidRBraket = 4;//无效的括号
    }

///语法分析器。仅分析一个模块，即从当前位置起，到第一个‘}’终止
    class cParser : public IBase
    {
    public:
        /** 构造函数。
            @param text : 待解析文本。
            @pram curNode : 当前模块结点。
            */
        cParser(StrStreamPtr stream, LZDataPtr curNode, int lineNo);
        ~cParser(void);

        ///开始分析
        void parse(void);

        ///获得行号
        int getLineNo(void) const { return m_lineNo;}

        ///是否有错误
        bool error(void) const { return m_errorNo != ErrorCode::None; }

        ///错误编号
        int getErrorNo(void) const { return m_errorNo; }

        ///错误行号
        int getErrorLine(void) const { return m_errorLine; }

        ///解析编码头
        static size_t parseCodingHeader(const char *buffer, size_t len);

        ///字符转义
        static tchar real2transChar(tchar src);

        static tchar trans2realChar(tchar src);

        ///字符串转义
        static void real2transString(tstring & dest, const tstring & src);

        static void trans2realString(tstring & dest, const tstring & src);

    protected:

        ///设置错误
        void setError(int eno, int line)
        {
            m_errorNo = eno;
            m_errorLine = line;
        }

        ///左大括号
        void onLBraket(void);

        ///右花括号
        void onRBraket(void);

        ///等号
        void onEqual(void);

        ///空白符
        void onSpace(void);

        ///回车符
        void onReturn(void);

        ///注释‘#’
        void onComment(void);

        ///分号
        void onSemicolon(void);

        ///其他字符
        void onOther(tchar ch);

        ///跳到语句结尾
        void skipToEnd(void);

        ///跳的回车符处
        void skipToReturn(void);

        ///从当前位置起，获得一个合法字符串。
        tstring getAStr(void);

    private:
        StrStreamPtr    m_stream;
        LZDataPtr       m_pCurNode;

        bool            m_bEnd;

        int             m_errorNo;
        int             m_errorLine;

        tstring         m_cacheName;
        tstring         m_tempStr;
        int             m_lineNo;
    };

}//namespace Lazy