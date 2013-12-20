#include "SharedResource.h"
#include "Action.h"

using namespace IStrategizer;

SharedResource::ResourceList SharedResource::s_resources;

void SharedResource::AddResource(SharedResource *p_pResource)
{
	ResourceList::const_iterator itr;

	itr = s_resources.find(p_pResource);

	if (itr != s_resources.end())
		throw ItemAlreadyExistsException(XcptHere);

	s_resources.insert(p_pResource);
}
//////////////////////////////////////////////////////////////////////////
void SharedResource::RemoveResource(SharedResource *p_pResource)
{
	ResourceList::const_iterator itr;

	itr = s_resources.find(p_pResource);

	if (itr == s_resources.end())
		throw ItemNotFoundException(XcptHere);

	s_resources.erase(itr);
}
//////////////////////////////////////////////////////////////////////////
void SharedResource::Lock(Action *p_pOwner)
{
	// Invalid owner
	if (p_pOwner == nullptr)
	{
		throw IStrategizer::InvalidParameterException(XcptHere);
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
			}
			else
				throw AcquireException(XcptHere);
			
		}
		// Recursive look
		else if (m_pOwner == p_pOwner)
		{
			throw RecursiveLockException(XcptHere);
		}
		// Already owned by someone else
		else
		{
			throw AlreadyLockedException(XcptHere);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void SharedResource::Unlock(Action *p_pOwner)
{
	// Invalid owner
	if (p_pOwner == nullptr)
	{
		throw IStrategizer::InvalidParameterException(XcptHere);
	}
	else
	{
		// No owner, so it is FREE
		if (m_pOwner == nullptr)
		{
			throw DifferentOwnerException(XcptHere);
		}
		// We are the owner, release the lock
		else if (m_pOwner == p_pOwner)
		{
			if (Release())
			{
				m_pOwner = nullptr;
				RemoveResource(this);
			}
			else
				throw ReleaseException(XcptHere);
		}
		//  Owned by someone else, we can't unlock it
		else
		{
			throw DifferentOwnerException(XcptHere);
		}
	}
}