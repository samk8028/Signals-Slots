#pragma once

#include <SignalsSlots/SignalsSlotsManager.h>


class AsSlot
{
public:
    inline virtual ~AsSlot()
    {
        SignalsSlotsManager::getInstance().onSlotDestroyed(this);
    }
};
