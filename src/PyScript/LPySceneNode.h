#pragma once

namespace Lzpy
{
    using namespace Lazy;

    class LPySceneNode : public PyBase
    {
        LZPY_DEF(LPySceneNode);
        
    public:

        LPySceneNode();
        ~LPySceneNode();


        LZPY_DEF_GETSET_MEMBER(position, m_entity->m_vPos);

        LZPY_DEF_GETSET_MEMBER(speed, m_entity->m_vSpeed);
        LZPY_DEF_GETSET_MEMBER(scale, m_entity->m_vScale);

    
    };

}// end namespace Lzpy
