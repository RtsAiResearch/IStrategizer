#ifndef MESSAGE_H
#define MESSAGE_H

#include "TypesMetaData.h"
#include "EngineObject.h"

namespace IStrategizer
{
    class Message : public EngineObject
    {
    public:
        Message(unsigned gameFrame, MessageType typeId) :
            m_gameFrame(gameFrame),
            m_typeId(typeId)
        {}

        unsigned GameFrame() const { return m_gameFrame; }
        MessageType TypeId() const { return m_typeId; }
        bool Delivered() const { return m_delivered; }
        void Delivered(bool delivered) { m_delivered = delivered; }
		const char* ToString()  const { return Enums[m_typeId]; }

	private:
		MessageType m_typeId;
		unsigned m_gameFrame;
		bool m_delivered;
    };
}

#endif // MESSAGE_H
