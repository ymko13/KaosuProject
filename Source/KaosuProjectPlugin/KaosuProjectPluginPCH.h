/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if !defined(GAMEPLUGINPCH_H_INCLUDED)
#define GAMEPLUGINPCH_H_INCLUDED

#if defined(WIN32)
  #define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#endif

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include "kSerializer.h"
#include "kBasicEntity.h"
#include "kStatObject.h"
#include "kCell.h"
#include "kPath.h"
#include "kRoom.h"
#include "kMap.h"

#include "kCharStats.h"
#include "kEffect.h"
#include "kDamageableEntity.h"
#include "kCharacter.h"
#include "kAiPath.h"

#include "kBaseComponent.h"
#include "kController.h"

extern VModule g_myComponentModule;

#endif

/*
 * Havok SDK - Base file, BUILD(#20131218)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
