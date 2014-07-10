#ifndef CONTAINER_H
#define CONTAINER_H

#include "ISerializable.h"

namespace Serialization
{
	class IContainer : public ISerializable
	{
	public:
		~IContainer() {}
		virtual int         ContainerCount() const = 0;
		virtual void        Clear() = 0;
		virtual void        AddTemp() = 0;
		virtual char*       GetTemp() = 0;
		virtual Iterator*	CreateIterator() const = 0;

	};
}
#endif // CONTAINER_H
