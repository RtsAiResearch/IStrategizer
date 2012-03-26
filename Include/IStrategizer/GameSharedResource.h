//#ifndef GAMESHAREDRESOURCE_H
//#define GAMESHAREDRESOURCE_H
//
//#include "IStrategizerException.h"
//
//class Action;
//
//namespace IStrategizer
//{
//	class GameSharedResource
//	{
//	public:
//		/*class LockException : public IStrategizer::Exception
//		{
//			enum Type { RecursiveLock, AlreadyAcquired };
//		};*/
//
//		GameSharedResource() : m_pOwner(0) {}
//		virtual ~GameSharedResource() {};
//
//		void Lock(Action *p_pOwner);
//		void Unlock(Action *p_pOwner);
//		bool IsLocked() { return m_pOwner == NULL; }
//	protected:
//		Action *m_pOwner;
//	};
//}
//
//#endif // GAMESHAREDRESOURCE_H