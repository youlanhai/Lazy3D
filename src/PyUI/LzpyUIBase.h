#pragma once

namespace Lzpy
{
    using namespace Lazy;

    class LzpyControl;

///实现ui的初始化方法
#define LZPY_IMP_INIT_LUI(CLASS, TYPE)    \
    LZPY_IMP_INIT(CLASS)                  \
    {                                       \
        return createUI(uitype::TYPE, arg); \
    }

    ///导出模块方法
    LZPY_DEF_MODULE(lui);

    //此类及其派生类，不得出现虚函数。
    class LzpyControl : public PyBase
    {
        LZPY_DEF(LzpyControl, PyBase);

    public:
        LzpyControl();
        ~LzpyControl();

        object getParent();

        object getSize();
        void setSize(object v);

        object getPosition();
        void setPosition(object v);

        object getRelativePos();
        void setRelativePos(object v);

        LZPY_DEF_GET(parent, getParent);
        LZPY_DEF_GET(type, m_control->getType);
        
        LZPY_DEF_GET(id, m_control->getID);
        LZPY_DEF_SET(id, m_control->setID, int);

        LZPY_DEF_GET(size, getSize);
        LZPY_DEF_SET(size, setSize, object);

        LZPY_DEF_GET(position, getPosition);
        LZPY_DEF_SET(position, setPosition, object);

        LZPY_DEF_GET(name, m_control->getName);
        LZPY_DEF_SET(name, m_control->setName, tstring);

        LZPY_DEF_GET(text, m_control->getText);
        LZPY_DEF_SET(text, m_control->setText, tstring);

        LZPY_DEF_GET(font, m_control->getFont);
        LZPY_DEF_SET(font, m_control->setFont, tstring);

        LZPY_DEF_GET(image, m_control->getImage);
        LZPY_DEF_SET(image, m_control->setImage, tstring);

        LZPY_DEF_GET(color, m_control->getColor);
        LZPY_DEF_SET(color, m_control->setColor, uint32);

        LZPY_DEF_GET(bgColor, m_control->getBgColor);
        LZPY_DEF_SET(bgColor, m_control->setBgColor, uint32);

        LZPY_DEF_GET(relative, m_control->getRelative);
        LZPY_DEF_SET(relative, m_control->setRelative, bool);

        LZPY_DEF_GET(relativePos, getRelativePos);
        LZPY_DEF_SET(relativePos, setRelativePos, object);

        LZPY_DEF_GET(relativeAlign, m_control->getRelativeAlign);
        LZPY_DEF_SET(relativeAlign, m_control->setRelativeAlign, uint32);

        LZPY_DEF_GET(visible, m_control->isVisible);
        LZPY_DEF_SET(visible, m_control->show, bool);

        LZPY_DEF_GET(enable, m_control->isEnable);
        LZPY_DEF_SET(enable, m_control->enable, bool);

        LZPY_DEF_GET(enableDrag, m_control->canDrag);
        LZPY_DEF_SET(enableDrag, m_control->enableDrag, bool);

        LZPY_DEF_GET(enableSelfMsg, m_control->isSelfMsgEnable);
        LZPY_DEF_SET(enableSelfMsg, m_control->enableSelfMsg, bool);

        LZPY_DEF_GET(enableClickTop, m_control->canClickTop);
        LZPY_DEF_SET(enableClickTop, m_control->enableClickTop, bool);

        LZPY_DEF_GET(enableChangeChildOrder, m_control->canChangeChildOrder);
        LZPY_DEF_SET(enableChangeChildOrder, m_control->enableChangeChildOrder, bool);

        LZPY_DEF_GET(editable, m_control->getEditable);
        LZPY_DEF_SET(editable, m_control->setEditable, bool);

        LZPY_DEF_GET(script, m_control->getScript);
        LZPY_DEF_SET(script, m_control->setScript, tstring);

        LZPY_DEF_GET_MEMBER(editorPool, m_editorPool);
        LZPY_DEF_SET_MEMBER(editorPool, m_editorPool);

        LZPY_DEF_GET_MEMBER(__dict__, m_pyDict);

        tuple getGlobalRect();
        void setGlobalRect(tuple rect);
        LZPY_DEF_GET(globalRect, getGlobalRect);
        LZPY_DEF_SET(globalRect, setGlobalRect, tuple);

        //坐标转换
        LZPY_DEF_METHOD(localToParent);
        LZPY_DEF_METHOD(parentToLocal);

        LZPY_DEF_METHOD(localToGlobal);
        LZPY_DEF_METHOD(globalToLocal);

        LZPY_DEF_METHOD(loadFromFile);
        LZPY_DEF_METHOD(saveToFile);


        LZPY_DEF_METHOD(addChild);
        LZPY_DEF_METHOD(delChild);
        LZPY_DEF_METHOD(getChild);
        LZPY_DEF_METHOD(getChildByName);

        LZPY_DEF_METHOD(addEditorChild);
        LZPY_DEF_METHOD(delEditorChild);

        LZPY_DEF_METHOD(clearChildren);
        LZPY_DEF_METHOD(getChildren);
        LZPY_DEF_METHOD(findChildByPos);
        LZPY_DEF_METHOD(destroy);

        bool createUI(int type, PyObject *arg = nullptr);
        bool addChild(object child);
        bool addToManage(object obj);
        void removeFromManage(object obj);

        PyObject    *m_pyDict;
        PyObject    *m_pyWeakreflist;
        list        m_editorPool;
        ControlPtr  m_control;
    };

}//end namespace Lzpy

