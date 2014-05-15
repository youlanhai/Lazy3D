#pragma once

namespace Lazy
{
    struct Bin
    {
        uint32 v;

        Bin() {}
        explicit Bin(uint32 v_) : v(v_){}
    };

    struct Oct
    {
        uint32 v;

        Oct(){}
        explicit Oct(uint32 v_) : v(v_){}
    };

    struct Hex
    {
        uint32 v;

        Hex() {}
        explicit Hex(uint32 v_) : v(v_) {}
    };


    bool parseString(int & v, const tstring & str);

    void buildString(tstring & str, char v);
    void buildString(tstring & str, uchar v);
    void buildString(tstring & str, int32 v);
    void buildString(tstring & str, uint32 v);
    void buildString(tstring & str, Bin v);
    void buildString(tstring & str, Oct v);
    void buildString(tstring & str, Hex v);

    template<typename Num>
    void buildStringNum(tstring & str, Num v, int base)
    {
        if (base <= 10) buildStringNumFast(str, v, base);
        else buildStringNumSlow(str, v, base);
    }

    template<typename Num>
    void buildStringNumFast(tstring & str, Num v, int base)
    {
        assert(base > 0);
        str.clear();

        if (v == 0)
        {
            str.push_back('0');
            return;
        }

        str.reserve(32);

        Num n;
        while (v != 0)
        {
            n = v % base;
            str.push_back('0' + n);
            v /= base;
        }

        std::reverse(str.begin(), str.end());
    }
    
    template<typename Num>
    void buildStringNumSlow(tstring & str, Num v, int base)
    {
        assert(base > 0);
        str.clear();

        if (v == 0)
        {
            str.push_back('0');
            return;
        }

        str.reserve(32);

        Num n;
        while (v != 0)
        {
            n = v % base;
            if (n < 10)
                str.push_back('0' + n);
            else
                str.push_back('A' + n - 10);
            v /= base;
        }

        std::reverse(str.begin(), str.end());
    }

    class StringConvTool
    {
    public:
        StringConvTool(void)
        {}

        StringConvTool(const tstring & v) 
            : m_value(v) 
        {}

        StringConvTool(const StringConvTool& st) 
            : m_value(st.m_value) 
        {}

        virtual ~StringConvTool(void)
        {}

        const StringConvTool& operator=(const StringConvTool & st)
        {
            m_value = st.m_value;
            return *this;
        }

        const StringConvTool& operator=(const tstring & v)
        {
            m_value = v;
            return *this;
        }

    public:

        int asInt() const;
        size_t asUint() const;
        size_t asHex() const;
        bool asBool() const;
        float asFloat() const;
        double asDouble() const;
        LPCTSTR asText() const{return m_value.c_str();}
        const tstring & asString() const{ return m_value; }

        void setInt(int data);
        void setUint(size_t data);
        void setHex(size_t data);
        void setBool(bool data);
        void setFloat(float data);
        void setDouble(double data);
        inline void setText(LPCTSTR data){ m_value = data; }
        inline void setString(const tstring& data){m_value = data;}

        bool formatFrom(LPCTSTR pFormat, ...);
        bool formatTo(LPCTSTR pFormat, ...);

    protected:
        tstring m_value;
    };

}//namespace Lazy