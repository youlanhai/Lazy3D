#pragma once

#include "Model.h"

namespace Lazy
{

    namespace ModelType
    {
        const int StaticModel = 0;
        const int SkinModel = 1;
    };


///加载模型
    class ModelFactory
    {
    public:
        ModelFactory();
        ~ModelFactory();

        ///使用主线程加载模型
        static ModelPtr loadModel(std::wstring const & filename, int type);

        ///使用后台线程加载模型
        template<typename F>
        static void loadModelBg(std::wstring const & filename, int type, F callback);
    };


///模型加载任务
    template<typename F>
    class ModelLoadTask : public TaskInterface
    {
        std::wstring    m_file;
        int             m_type;
        F               m_callback;
        ModelPtr        m_model;

    public:

        ModelLoadTask(const std::wstring & filename, int type, F callback)
            : m_file(filename)
            , m_type(type)
            , m_callback(callback)
        { }

        virtual void doTask(void) override
        {
            m_model = ModelFactory::loadModel(m_file, m_type);
        }

        virtual void onTaskFinish(void) override
        {
            m_callback(m_model);
        }
    };


    template<typename F>
    /*static*/ void ModelFactory::loadModelBg(std::wstring const & filename, int type, F callback)
    {
        TaskPtr task = new ModelLoadTask<F>(filename, type, callback);

#ifdef USE_MULTI_THREAD
        LoadingMgr::instance()->addTask(task);
#else
        task->doTask();
        task->onTaskFinish();
#endif

    }

} // end namespace Lazy
