#pragma once

#include "Base.h"

namespace Lazy
{

    struct LRC
    {
        int time;           //当前歌词时间
        std::string lyrics; //当前歌词。
        LRC(void) : time(0) {}
        LRC(int t, char *s) : time(t), lyrics(s) {}
    };

//歌词加载器
    class LRCLoader : public IBase
    {
    public:
        LRCLoader(void);
        ~LRCLoader(void);
        bool load(char *szFileName);
        void analysis(char *str);
        void clear(void);

        inline LRC * at(int index)
        {
            if(index >= 0 && (UINT)index < m_vecLRC.size())
            {
                m_nCurIndex = index;
                return &(m_vecLRC.at(index));
            }
            else return NULL;
        }
        inline LRC * atCur(void) { return at(m_nCurIndex); }
        inline LRC * atNext( void ) { return at(++m_nCurIndex); }
        inline std::vector<LRC> & getVec( void ) { return m_vecLRC; }

        inline void setCurIndex(int index) { m_nCurIndex = index; }
        inline int getCurIndex( void ) { return m_nCurIndex; }
    private:
        std::vector<LRC> m_vecLRC;
        int     m_nCurIndex;//当前已访问的索引。
    };


} // end namespace Lazy
