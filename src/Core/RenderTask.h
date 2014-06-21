#pragma once

#include "RenderObj.h"

namespace Lazy
{
    ///安全访问数据。注意：该类不支持多线程。
    class CRenderTask :
        public IRenderObj,
        public VisitPool<IRenderObj*>
    {
    public:

        void update(float fElapse)
        {
            lock();
            for (iterator it = begin(); it != end(); ++it)
            {
                if (*it)
                {
                    (*it)->update(fElapse);
                }
            }
            unlock();
        }

        void render(IDirect3DDevice9 * pDevice)
        {
            lock();
            for (iterator it = begin();  it != end();  ++it )
            {
                if (*it)
                {
                    (*it)->render(pDevice);
                }
            }
            unlock();
        }
    };

    typedef RefPtr<CRenderTask> RenderTaskPtr;


} // end namespace Lazy
