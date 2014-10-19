
#include "stdafx.h"
#include "LRCLoader.h"

namespace Lazy
{

    LRCLoader::LRCLoader(void)
    {
        m_nCurIndex = -1;
        m_vecLRC.reserve(100);
    }

    LRCLoader::~LRCLoader(void)
    {
    }

    bool LRCLoader::load(char *szFileName)
    {
        clear();
        std::ifstream fin(szFileName);
        if(fin.bad() || fin.fail())
        {
            return false;
        }
        char szBuffer[256];
        while(!fin.eof())
        {
            fin.getline(szBuffer, 256);
            analysis(szBuffer);
        }
        fin.close();
        return true;
    }

    void LRCLoader::analysis(char *str)
    {
        int bracket = 0; //括号。-1左括号，0无括号，1右括号
        LRC lrc;
        char ch;
        int tFlag = 0;  //时间标识。1分,2秒,3毫秒.
        int t = 0;      //保存临时时间
        while((*str) != 0)
        {
            ch = *str;
            if (ch == '[')
            {
                if(bracket == 0)
                {
                    bracket = -1;
                }
                else break;//异常。出现了第二个'['
            }
            else if(ch == ']')//正常结束
            {
                bracket = 1;

                lrc.time += t * 10; //最后读出的时间单位是10ms
                lrc.lyrics = (str + 1);
                m_vecLRC.push_back(lrc);

                break; //正常结束
            }
            else if(ch >= 48 && ch < 58)
            {
                t = t * 10 + ch - 48;
            }
            else if(ch == ':')
            {
                if (tFlag == 0)
                {
                    tFlag = 1;
                    lrc.time += t * 60000;  //1分钟=60秒=60000毫秒
                    t = 0;
                }
                else break; //异常。出现了第二个':'。
            }
            else if(ch == '.')
            {
                if( tFlag == 1)
                {
                    lrc.time += t * 1000; //1s=1000ms
                    t = 0;
                }
                else break; //异常。出现了第二个'.'。
            }
            else
            {
                break;  //异常。非法字符。
            }

            ++str;
        }
    }

    void LRCLoader::clear(void)
    {
        m_nCurIndex = -1;
        m_vecLRC.clear();
    }

} // end namespace Lazy
