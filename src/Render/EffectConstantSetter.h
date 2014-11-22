#ifndef H_EFFECT_AUTO_CONST_H
#define H_EFFECT_AUTO_CONST_H

#include "EffectConstant.h"

namespace Lazy
{
    /** 根据Effect变量的语义，为变量设置数据。
     *  比如：世界矩阵、观察矩阵、投影矩阵等。
     */
    class EffectConstantSetter
    {
    public:
        EffectConstantSetter();
        virtual ~EffectConstantSetter();

        virtual void apply(EffectConstant *pConst) = 0;

        static void init();
        static void fini();

        static EffectConstantSetter * get(const std::string & name);
        static void set(const std::string & name, EffectConstantSetter *autoConst);

    private:
        static std::map<std::string, EffectConstantSetter*> s_autoConstMap;
    };
    
}//end namespace Lazy


#endif //H_EFFECT_AUTO_CONST_H
