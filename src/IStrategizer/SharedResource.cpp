#include "SharedResource.h"
#include "EngineObject.h"
#include "Logger.h"

using namespace IStrategizer;

SharedResource::ResourceList SharedResource::s_resources;

void SharedResource::AddResource(SharedResource *p_pResource)
{
    ResourceList::const_iterator itr;

    itr = s_resources.find(p_pResource);

    if (itr != s_resources.end())
        DEBUG_THROW(ItemAlreadyExistsException(XcptHere));

    s_resources.insert(p_pResource);
}
//////////////////////////////////////////////////////////////////////////
void SharedResource::RemoveResource(SharedResource *p_pResource)
{
    ResourceList::const_iterator itr;

    itr = s_resources.find(p_pResource);

    if (itr == s_resources.end())
        DEBUG_THROW(ItemNotFoundException(XcptHere));

    s_resources.erase(itr);
}
//////////////////////////////////////////////////////////////////////////
SharedResource::~SharedResource()
{ 
    if (IsLocked())
    {
        LogWarning("Resource 0x%x leaked, will remove it from resource list anyway", (void*)this);
        RemoveResource(this);
    }
};
//////////////////////////////////////////////////////////////////////////
void SharedResource::Lock(EngineObject *p_pOwner)
{
    // Invalid owner
    if (p_pOwner == nullptr)
    {
        DEBUG_THROW(IStrategizer::InvalidParameterException(XcptHere));
    }
    else
    {
        // No owner, so it is FREE, acquire the lock
        if (m_pOwner == nullptr)
        {
            if (Acquire())
            {
                m_pOwner = p_pOwner;
                AddResource(this);
                LogInfo("Action '%s' acquired resource '%s'", m_pOwner->ToString().c_str(), ToString().c_str());
            }
            else
                DEBUG_THROW(AcquireException(XcptHere));

        }
        // Recursive look
        else if (m_pOwner == p_pOwner)
        {
            DEBUG_THROW(RecursiveLockException(XcptHere));
        }
        // Already owned by someone else
        else
        {
            DEBUG_THROW(AlreadyLockedException(XcptHere));
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void SharedResource::Unlock(EngineObject *p_pOwner)
{
    // Invalid owner
    if (p_pOwner == nullptr)
    {
        DEBUG_THROW(IStrategizer::InvalidParameterException(XcptHere));
    }
    else
    {
        // No owner, so it is FREE
        if (m_pOwner == nullptr)
        {
            DEBUG_THROW(DifferentOwnerException(XcptHere));
        }
        // We are the owner, release the lock
        else if (m_pOwner == p_pOwner)
        {
            if (Release())
            {
                LogInfo("Action '%s' released resource '%s'", m_pOwner->ToString().c_str(), ToString().c_str());
                m_pOwner = nullptr;
                RemoveResource(this);
            }
            else
                DEBUG_THROW(ReleaseException(XcptHere));
        }
        //  Owned by someone else, we can't unlock it
        else
        {
            DEBUG_THROW(DifferentOwnerException(XcptHere));
        }
    }
}
//////////////////////////////////////////////////////////////////////////
std::string SharedResource::ToString(bool minimal) const
{
	char buff[512];
	sprintf_s(buff, "Owner=%s", (m_pOwner != nullptr ? m_pOwner->ToString().c_str() : ""));
	
	return buff;
}

