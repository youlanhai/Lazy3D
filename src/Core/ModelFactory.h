#pragma once

#include "Model.h"

namespace Lazy
{
    ///加载模型
    class ModelFactory
    {
    public:
        ModelFactory();
        ~ModelFactory();

        ///使用主线程加载模型
        static ModelPtr loadModel(std::wstring const & filename);

        ///使用后台线程加载模型
        template<typename F>
        static void loadModelBg(std::wstring const & filename, F callback);
    };


    ///模型加载任务
    template<typename F>
    class ModelLoadTask : public TaskInterface
    {
        std::wstring    m_file;
        F               m_callback;
        ModelPtr        m_model;

    public:

        ModelLoadTask(const std::wstring & filename, F callback)
            : m_file(filename)
            , m_callback(callback)
        { }

        virtual void doTask(void) override
        {
            m_model = ModelFactory::loadModel(m_file);
        }

        virtual void onTaskFinish(void) override
        {
            m_callback(m_model);
        }
    };


    template<typename F>
    /*static*/ void ModelFactory::loadModelBg(std::wstring const & filename, F callback)
    {
        TaskPtr task = new ModelLoadTask<F>(filename, callback);

#ifdef USE_MULTI_THREAD
        LoadingMgr::instance()->addTask(task);
#else
        task->doTask();
        task->onTaskFinish();
#endif

    }

} // end namespace Lazy
