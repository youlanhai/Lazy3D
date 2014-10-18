#pragma once


namespace Lazy
{
    class SceneNode;

    /** 鼠标拾取*/
    class LZDLL_API Pick :
        public IBase,
        public Singleton<Pick>
    {
    public:
        Pick();
        ~Pick();

        ///处理windows消息
        bool handeEvent(const SEvent & event);

        ///获得与地图的交点坐标
        const Vector3 & getPosOnTerrain(void) const { return m_posOnTerrain; }

        ///鼠标是否与地图相交
        bool isIntersectWithTerrain() const { return m_intersectWithTerrain; }

        ///是否与包围球相交。
        bool isIntersectWithSphere(const Vector3 & center, float radius);

        ///与aabb相交测试
        bool isIntersect(const AABB & aabb);

        ///与物体相交测试
        bool isIntersect(SceneNode * pObj);

        ///与地图物件相交是否启用
        bool isTerrainObjEnable(void) const { return m_terrainObjEnable; }

        ///启/禁用与地图物件相交
        void setTerrainObjEnable(bool e) { m_terrainObjEnable = e; }

        ///获取射线原点
        const Vector3 & rayPos() const { return m_pos; }

        ///获取射线方向
        const Vector3 & rayDir() const { return m_dir; }

    public:

        ///将鼠标坐标转换为3维射线
        void convertCursor(int x, int y);

        ///查询与entity相交情况
        bool queryWithEntity();

        ///查询与地图物件相交情况
        bool queryWithTerrainObj();

        ///查询与地表相交情况
        bool queryWithTerrain();

    protected:
        CPoint              m_cursor;///<鼠标坐标
        Vector3             m_pos;///<射线原点
        Vector3             m_dir;///<射线方向
        Vector3             m_posOnTerrain;///<与地形交点
        bool                m_intersectWithTerrain;
        bool                m_terrainObjEnable;
    };

} // end namespace Lazy
