#pragma once

#include <SignalsSlots/SlotBase.h>
#include <SignalsSlots/SignalsSlotsManager.h>

#include <functional>


template<typename... Args_t>
class Slot : public SlotBase
{
private:
    template<typename T>
    using adjustedType = std::conditional_t<std::is_rvalue_reference_v<T> , T , std::add_lvalue_reference_t<std::remove_reference_t<T>>>;


public:
    inline Slot(const std::function<void(Args_t...)>& func) : SlotBase(false)
    {
        setCallback(func);
    }

    inline Slot(std::function<void(Args_t...)>&& func) : SlotBase(false)
    {
        setCallback(std::move(func));
    }

    inline ~Slot()
    {
        if(isHeapAllocated() == false)
        {
            SignalsSlotsManager::getInstance().onSlotDestroyed(this);
        }
    }

    inline void setCallback(const std::function<void(Args_t...)>& func)
    {
        _callback = func;
    }

    inline void setCallback(std::function<void(Args_t...)>&& func)
    {
        _callback = std::move(func);
    }

    inline void operator()(Args_t... args)
    {
        _callback(std::forward<Args_t>(args)...);
    }


private:
    std::function<void(adjustedType<Args_t>...)> _callback;
};
