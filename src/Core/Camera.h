#pragma once

#include "SceneNode.h"

namespace Lazy
{

    /*  玩家（player）控制相机旋转规则。
        1.第一人称，相机完全有玩家控制。
        2.自由模式，相机完全“自己”控制。
        3.第3人称，根据需要，进行玩家和自己切换。
    */

    /** 鼠标控制摄像机。*/
    class LZDLL_API CCursorCamera :  public SceneNode
    {
    public:
        enum CameraType //摄像机样式
        {
            FIRST = 1,	//第一人称
            THIRD = 2,	//第三人称
            FREE = 3,  //自由模式
        };

        explicit CCursorCamera(CameraType type = THIRD);
        ~CCursorCamera(void);

        virtual bool handleEvent(const SEvent & event);

        virtual void update(float elapse);
        virtual void render(dx::Device *) {}

        /** 设置鼠标旋转速度。单位为：弧度/像素*/
        void setCurRoSpeed(float speed);
        float getRotateSpeed() { return m_curSpeedX; }

        void showCursor(bool show);

        virtual void drag(CPoint pt);
        bool isDraged(void) { return m_draged; }

        void setSource(SceneNode *p) { m_pSource = p; }
        SceneNode* getSource(void) { return m_pSource; }

        void setHeight(float h) { m_height = h; }
        void setCamareType(CameraType cameraType);
        CameraType getCameraType(void) { return m_cameraType; }

        void setDistRange(float mind, float maxd);

        Matrix getViewMatrix() const;
        const Matrix & getInvViewMatrix() const{ return getMatrix(); }

        float getSpeed() const { return m_speed; }
        void setSpeed(float speed){ m_speed = speed; }

    protected:

        /** 矫正距离玩家的距离。*/
        void correctDist(void);

    protected:

        SceneNode*  m_pSource;
        CameraType  m_cameraType;//照相机类型
        float		m_fDistToPlayer;//照相机离玩家的距（跟随玩家移动用）
        float       m_realDistToPlayer;
        float       m_distMin;
        float       m_distMax;
        float       m_height;
        float       m_speed;
        float       m_lastElapse;

        CPoint      m_ptDown;
        float       m_curSpeedX;
        float       m_curSpeedY;
        bool        m_bMouseDown;
        bool        m_bCurShow;
        bool        m_draged;
    };

    LZDLL_API CCursorCamera* getCamera(void);

} // end namespace Lazy
