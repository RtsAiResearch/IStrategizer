#ifndef STATICCOMPONENT_H
#define STATICCOMPONENT_H

namespace IStrategizer
{
    class StaticComponent
    {
    protected:
        bool m_initalized;
        virtual void    InitializeAux() = 0;
    public:
                        StaticComponent() : m_initalized(false) {}
        virtual         ~StaticComponent();
        void            Initialize();
    };
}


#endif // STATICCOMPONENT_H