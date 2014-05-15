#pragma once


namespace Physics
{
    //计算矩阵的缩放系数，xyz的缩放系数必须相等，否则结果无意义。
    float getMatrixScale(const Matrix4x4 & matrix);

    void splitAABB(Plane & p, AABB & front, AABB & back, const AABB & aabb, int axis);

    void calTrianglesAABB(
        AABB & aabb, 
        const TriangleIndices & indices, 
        const TriangleSet & triangles
        );

    //////////////////////////////////////////////////////////////////////////
    //d3dmesh三角形提取
    //////////////////////////////////////////////////////////////////////////
    class MeshExtractor
    {
    public:

        MeshExtractor();

        ~MeshExtractor();

        bool build(ID3DXMesh *pMesh);

        bool valid() const { return m_isValid; }

        size_t nbTriangle() const { return m_triangles.size(); }

        void getTriangle(Triangle & tri, int index) const
        {
            tri = m_triangles[index];
        }

        const Triangle & getTriangle(int index) const
        {
            return m_triangles[index];
        }

        const TriangleSet & triangles() const{ return m_triangles; }

        virtual size_t getBytes() const;
    private:
        bool        m_isValid;
        TriangleSet m_triangles;
    };

}