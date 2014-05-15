#pragma once

namespace Lazy
{
    typedef char dschar;
    typedef std::vector<dschar> streambuffer;
	
	class DataStream
	{
	public:
		DataStream();
		~DataStream();

		dschar get();
		void append(dschar ch);

		void gets(void *dest, size_t len);
		void appends(const void *src, size_t leng);
        
	    //跳过len个字节
        void skip(size_t len);
        
		inline bool empty() const { return pos_ >= stream_.size(); }
		
        size_t pos() const { return pos_; }
        streambuffer & steam(){ return stream_; }
        const dschar * buffer() const { stream_.data(); }

        void clear();

        ///将数据存到文件
        bool writeToFile(FILE *pFile);
        bool readFromFile(FILE *pFile, size_t n);

	public:

		template<class T>
		void loadStruct(T & t)
		{
			gets(&t, sizeof(t));
		}

		template<class T>
		T loadStruct()
		{
			T t;
			loadStruct(t);
			return t;
		}

		template<class T>
		void saveStruct(const T & t)
		{
			appends(&t, sizeof(t));
		}

        template<class T>
		void loadString(T & str)
		{
			size_t len;
			loadStruct(len);
			str.resize(len);
			if (len > 0) gets(&str[0], len * sizeof(str[0]));
		}

        template<class T>
		void saveString(const T & str)
		{
			size_t len = str.length();
			saveStruct(len);
			if (len > 0) appends(str.c_str(), len * sizeof(str[0]));
		}

	private:
		streambuffer	stream_;
		size_t			pos_;
	};

}