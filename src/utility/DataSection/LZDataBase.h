//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "DataSectionConfig.h"

#include "../UtilConfig.h"
#include "../SmartPtr.h"
#include "../StringConvTool.h"
#include "../Misc.h"
#include "../MemoryPool.h"

#define IMP_READ_FUN(name, type, defV)    \
type read##name(const tstring & tag, type def=defV)\
{\
    LZDataPtr ptr = read(tag);\
    if (!ptr) return def;\
    return ptr->as##name();\
}

#define IMP_WRITE_FUN(name, type) \
void write##name(const tstring & tag, type data)\
{\
    write(tag)->set##name(data);\
}

namespace Lazy
{
    ///数据类型
    enum class DataType
    {
        Default = 0,
        Lzd,
        Xml,

        /* 以下类型暂不支持
        Ini,
        Binary,
        Folder,
        Zip,
        */
    };

    typedef RefPtr<class LZDataBase> LZDataPtr;

    class LZDATA_API LZDataBase : public StringConvTool, public IBase
    {
    protected:
        LZDataBase(void);
    
        explicit LZDataBase(const tstring & tag);
    
        LZDataBase(const tstring & tag, const tstring & value_);

    public:

        virtual ~LZDataBase(void);

        DEC_MEMORY_ALLOCATER()

    public:
        typedef std::vector<LZDataPtr>      DataPool;
        typedef DataPool::iterator          DataIterator;
        typedef DataPool::const_iterator    DataConstIterator;

    public: //对自身的操作

        void setTag(const tstring &tag){ m_tag = tag; }
        const tstring& tag() const { return m_tag; }
        const tchar* ctag() const { return m_tag.c_str(); }

        void setValue(const tstring & v){ m_value = v; }
        const tstring & value() const { return m_value; }
        const tchar* cvalue()const { return m_value.c_str(); }

    public://对子孙节点的操作

        IMP_READ_FUN(Int, int, 0);
        IMP_READ_FUN(Uint, size_t, 0);
        IMP_READ_FUN(Hex, size_t, 0);
        IMP_READ_FUN(Bool, bool, false);
        IMP_READ_FUN(Float, float, 0.0f);
        IMP_READ_FUN(Double, double, 0.0);
        IMP_READ_FUN(String, const tstring&, EmptyStr);

        IMP_WRITE_FUN(Int, int);
        IMP_WRITE_FUN(Uint, size_t);
        IMP_WRITE_FUN(Hex, size_t);
        IMP_WRITE_FUN(Bool, bool);
        IMP_WRITE_FUN(Float, float);
        IMP_WRITE_FUN(Double, double);
        IMP_WRITE_FUN(String, const tstring&);

    public://接口

        virtual LZDataPtr newOne(const tstring & tag, const tstring & value_) = 0;

        virtual bool loadFromBuffer(const tchar* buffer, int length) = 0;

        virtual void print(tostream & out, int depth=0) = 0;

    public:
        DataIterator begin(){ return m_children.begin(); }
        DataConstIterator begin()const { return m_children.begin();}

        DataIterator end(){ return m_children.end(); }
        DataConstIterator end() const { return m_children.end(); }

        //获得子孙结点属性。支持a/b/c的形式。
        virtual LZDataPtr read(const tstring & tag);

        virtual LZDataPtr write(const tstring & tag, tstring value_=_T(""));

        virtual const DataPool& children(void){ return m_children; }

        void clearChildren(){ m_children.clear(); }

        virtual int countChildren(void){ return (int)m_children.size(); }

        virtual int addChild(LZDataPtr ptr){ m_children.push_back(ptr); return countChildren()-1;}

        virtual LZDataPtr getChild(int index){ return m_children[index]; }

        virtual int findChild(const tstring & name){ return findNextChild(-1, name); }

        virtual int findNextChild(int index, const tstring& name);

    public://存档操作

        virtual bool load(const tstring & fname);

        virtual bool save(const tstring & fileName) ;

    protected:
        tstring     m_tag;
        DataPool    m_children;
    };

    void printTable(tostream & cout, int n);

}//namespace Lazy