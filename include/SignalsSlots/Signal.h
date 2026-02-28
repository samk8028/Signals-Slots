#pragma once

#include <SignalsSlots/SignalsSlotsManager.h>

template<typename... Args_t>
class Signal
{
public:
    inline ~Signal()
    {
        SignalsSlotsManager::getInstance().onSignalDestroyed(this);
    }

    inline void operator()(Args_t... args)
    {
        SignalsSlotsManager::getInstance().onSignalEmited(this , adjustParametersTypes<Args_t>(args)...);
    }


private:
    template<typename T , typename U>
    inline decltype(auto) adjustParametersTypes(U&& arg)
    {
        if constexpr(std::is_rvalue_reference_v<T>)
        {
            return std::move(arg);
        }
        else
        {
            return arg;
        }
    }
};
