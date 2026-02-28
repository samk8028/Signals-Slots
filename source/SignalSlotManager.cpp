#include <SignalsSlots/Signal.h>
#include <SignalsSlots/Slot.h>
#include <SignalsSlots/SignalsSlotsManager.h>


SignalsSlotsManager::SignalsSlotsManager() = default;

SignalsSlotsManager::~SignalsSlotsManager()
{
    for(const auto& connection : _signalsSlotsRegistery)
    {
        for(const auto& tuple : connection.second)
        {
            if(const auto slot = static_cast<SlotBase*>(std::get<SlotObjPtr_Index>(tuple)) ; slot->isHeapAllocated())
            {
                delete slot;
            }
        }
    }
}

void SignalsSlotsManager::onSignalDestroyed(SignalPtr signal)
{
    _signalsSlotsRegistery.erase(signal);
}

void SignalsSlotsManager::onSlotDestroyed(SlotObjPtr slot)
{
    for(auto& connection : _signalsSlotsRegistery)
    {
        for(auto it = connection.second.cbegin() ; it != connection.second.cend();)
        {
            if(std::get<SlotObjPtr_Index>(*it) == slot)
            {
                it = connection.second.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}

void SignalsSlotsManager::onSlotDestroyed(AsSlot* asSlot)
{
    for(auto& connection : _signalsSlotsRegistery)
    {
        for(auto it = connection.second.cbegin() ; it != connection.second.cend();)
        {
            if(std::get<AsSlotPtr_Index>(*it) == asSlot)
            {
                delete static_cast<SlotBase*>(std::get<SlotObjPtr_Index>(*it));

                it = connection.second.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}

SignalsSlotsManager& SignalsSlotsManager::getInstance() noexcept
{
    static SignalsSlotsManager instance;

    return instance;
}
