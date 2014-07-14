#ifndef ARMY_H
#define ARMY_H

#include "EngineObject.h"
#include "EngineData.h"

namespace IStrategizer
{
    class RtsGame;
    class WorldClock;
    class Vector2;

    class Army : public EngineObject
    {
    public:
        Army(RtsGame& game);
        ~Army();
        void NotifyMessegeSent(Message* p_msg);
        Vector2 Center() const;
        EntitySet Entities() const { return m_army; }
        bool Empty() const { return m_army.empty(); }

    private:
        void SelectArmy(RtsGame& game);

        EntitySet m_army;
    };
}

#endif // ARMY_H