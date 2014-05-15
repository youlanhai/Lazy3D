#include "ModelView.h"

namespace Lazy
{


    ModelView::ModelView()
    {
        setBgColor(0x7f000000);

        CLabel * pTitle = new CLabel();
        pTitle->create(0, L"model view", 0, 10);
        addChild(pTitle);
        getGUIMgr()->addToManagePool(pTitle);


    }


    ModelView::~ModelView()
    {
    }

}