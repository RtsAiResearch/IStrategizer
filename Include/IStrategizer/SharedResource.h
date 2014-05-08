#ifndef SHAREDRESOURCE_H
#define SHAREDRESOURCE_H

#include "IStrategizerException.h"
#include <set>

namespace IStrategizer
{
    class Action;

    class SharedResource
    {
    public:
        typedef std::set<SharedResource*> ResourceList;

        class AlreadyLockedException : public IStrategizer::Exception
        {
        public:
            AlreadyLockedException(ExceptionLocation p_location)
                : Exception(p_location, "AlreadyLockedException") {}
        };

        class DifferentOwnerException : public IStrategizer::Exception
        {
        public:
            DifferentOwnerException(ExceptionLocation p_location)
                : Exception(p_location, "DifferentOwnerException") {}
        };

        class RecursiveLockException : public IStrategizer::Exception
        {
        public:
            RecursiveLockException(ExceptionLocation p_location)
                : Exception(p_location, "RecursiveLockException") {}
        };

        class AcquireException : public IStrategizer::Exception
        {
        public:
            AcquireException(ExceptionLocation p_location)
                : Exception(p_location, "AcquireException") {}
        };

        class ReleaseException : public IStrategizer::Exception
        {
        public:
            ReleaseException(ExceptionLocation p_location)
                : Exception(p_location, "ReleaseException") {}
        };

        SharedResource() : m_pOwner(0) {}
        virtual ~SharedResource() {};

        void Lock(Action *p_pOwner) throw(
            IStrategizer::InvalidParameterException,
            AcquireException,
            RecursiveLockException,
            AlreadyLockedException);

        virtual std::string ToString() const { return m_rescourceDescription; }
        void Unlock(Action *p_pOwner);
        bool IsLocked() { return m_pOwner != nullptr; }
        const Action* Owner() const { return m_pOwner; }
        virtual bool IsNull() = 0;

        static void AddResource(SharedResource *p_pResource) throw(IStrategizer::ItemAlreadyExistsException);
        static void RemoveResource(SharedResource *p_pResource) throw(IStrategizer::ItemNotFoundException);
        static const ResourceList& LockedResources() { return s_resources; }

    protected:
        virtual bool Acquire() = 0;
        virtual bool Release() = 0;

        Action *m_pOwner;
        std::string m_rescourceDescription;
        static ResourceList s_resources;
    };
}

#endif // SHAREDRESOURCE_H
