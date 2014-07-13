#pragma once

#include "RenderObj.h"

namespace Lazy
{

    class LZDLL_API I3DObject :
        public IBase,
        public IRenderObj
    {
    public:
        I3DObject(void);
        ~I3DObject();

        void moveLook(float fElapse);	//前进
        void moveUp(float fElapse);		//上升
        void moveRight(float fElapse);	//平移

        void rotationUp(float angle);		//绕up旋转
        void rotationLook(float angle);	//绕Look旋转
        void rotationRight(float angle);	//绕right旋转

        /*绕给定向量旋转*/
        void rotationAxis(const Vector3 & a, float angle );

        /*绕X、Y、Z轴旋转*/
        void rotationX(float angle);
        void rotationY(float angle);
        void rotationZ(float angle);

        //重置方向
        void resetdir();

        virtual void getWorldMatrix(Matrix & mat) const;//获得世界矩阵
        virtual void getCameraMatrix(Matrix & mat) const;//获得相机矩阵

        virtual Matrix getRotationMatrix();//获得旋转矩阵
        virtual Matrix getRotationMatrix2();
        virtual void normalizeVector(void);

        virtual void update(float) {}
        virtual void render(IDirect3DDevice9 *) {}

    public://属性
        const Vector3& getPos(void) const { return m_vPos; }
        void setPos( const Vector3 &pos ) { m_vPos = pos; }

        const Vector3& getLook(void ) const { return m_vLook; }
        void setLook( const Vector3 &look ) { m_vLook = look; }

        const Vector3& getUp( void ) const { return m_vUp; }
        void setUp( const Vector3 &up ) { m_vUp = up; }

        const Vector3& getRight( void ) const { return m_vRight; }
        void setRight( const Vector3 &right ) { m_vRight = right; }

        const Vector3 & getScale() const { return m_vScale; }
        void setScale( float s) { m_vScale.set(s, s, s); }
        void setScale(const Vector3 &v) { m_vScale = v; }

        const Vector3 & getSpeed() const { return m_vSpeed; }
        void setSpeed(float s) { m_vSpeed.set(s, s, s); }
        void setSpeed(const Vector3 &v) { m_vSpeed = v; }

        const Vector3 & getRotSpeed() const { return m_vRotSpeed; }
        void setRotSpeed(float s) { m_vRotSpeed.set(s, s, s);}
        void setRotSpeed(const Vector3 &v) { m_vRotSpeed = v;}

        const AABB & getAABB() const { return m_aabb; }
        void setAABB(const AABB & a) { m_aabb = a;}
        void getWorldAABB(AABB & a) const;

    public:
        /** 是否是玩家*/
        virtual bool isPlayer(void) { return false; }

        bool visible(void) { return m_visible; }
        virtual void show(bool s) { m_visible = s; }

        void enableLockHeight(bool l) { m_lockHInMap = l; }
        void setLockHeight(float h) { m_lockHeight = h ;}
        float getLockHeight() const { return m_lockHeight; }

    public:
        Vector3    m_vPos;			//位置
        Vector3    m_vLook;		//朝向，对应z轴
        Vector3    m_vUp;			//上向量,对应y轴
        Vector3    m_vRight;		//右向量，对应x轴
        Vector3    m_vRotSpeed;    //旋转速度
        Vector3    m_vScale;       //缩放
        Vector3    m_vSpeed;       //移动速度
        AABB       m_aabb;         //包围盒

    protected:
        bool    m_visible;
        bool    m_lockHInMap;
        float   m_lockHeight;
    };


} // end namespace Lazy