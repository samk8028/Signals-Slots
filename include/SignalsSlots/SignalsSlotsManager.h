#pragma once

#include <SignalsSlots/LibraryCreationUtilities.h>
#include <SignalsSlots/SlotBase.h>

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <mutex>


typedef void* SlotObjPtr;    // SlotObj_Ptr is used where an instance of the Slot<Args_t...> class is usded directly as a slot.
typedef void* SignalPtr;

class AsSlot;

template<typename... Args_t>
class Slot;


class LIBRARY_API SignalsSlotsManager
{
    enum Type
    {
        Multi  = 0 ,
        Single = 1
    };


    // This enum is used to get the tuple elemnts of _signalsSlotsRegistery.
    enum
    {
        SlotObjPtr_Index     = 0 ,
        AsSlotPtr_Index      = 1 ,
        SlotType_Index       = 2 ,
        ConnectionType_Index = 3
    };


public:
    enum SlotType
    {
        SlotObject     = 0 ,
        MemberFunction = 1
    };


private:
    template<typename FunctionPtr_t>
    inline static std::vector<SlotObjPtr> _funcPtrToSlot;

    std::unordered_map<SignalPtr , std::vector<std::tuple<SlotObjPtr , AsSlot* , SlotType , Type>>> _signalsSlotsRegistery;

    std::mutex _mutex;


private:
    SignalsSlotsManager();

    template<typename FunctionPtr_t>
    decltype(SignalsSlotsManager::_signalsSlotsRegistery) getSameConnections(SignalPtr signalToCheck, SlotObjPtr slotToCheck , AsSlot* asSlot , FunctionPtr_t func) const;

    template<typename FunctionPtr_t>
    inline bool checkIfSameConnectionExists(SignalPtr signalToCheck, SlotObjPtr slotToCheck, AsSlot *asSlot, FunctionPtr_t func) const;

    template<typename FunctionPtr_t>
    void connectSignalSlot(SignalPtr signal, SlotObjPtr slotObj , AsSlot* asSlot , FunctionPtr_t func , SlotType slotType , Type connectionType);

    template<typename FunctionPtr_t>
    void disconnectSignalSlot(SignalPtr signal , SlotObjPtr slot , AsSlot* asSlot , FunctionPtr_t func , Type type);


public:
    ~SignalsSlotsManager();

    void onSignalDestroyed(SignalPtr signal);

    void onSlotDestroyed(SlotObjPtr slot);

    void onSlotDestroyed(AsSlot* asSlot);

    static SignalsSlotsManager& getInstance() noexcept;


public:
    template<typename... Args_t>
    inline void onSignalEmited(SignalPtr signal, Args_t&&... args);


public:
    friend class SignalsSlots;
};


//


template<typename FunctionPtr_t>
inline decltype(SignalsSlotsManager::_signalsSlotsRegistery) SignalsSlotsManager::getSameConnections(SignalPtr signalToCheck, SlotObjPtr slotToCheck, AsSlot* asSlot, FunctionPtr_t func) const
{
    decltype(_signalsSlotsRegistery) connections;

    if(asSlot == nullptr)
    {
        if(const auto it = _signalsSlotsRegistery.find(signalToCheck) ; it != _signalsSlotsRegistery.cend())
        {
            for(const auto& tuple : it->second)
            {
                if(slotToCheck == std::get<SlotObjPtr_Index>(tuple))
                {
                    connections.emplace(signalToCheck , tuple);
                }
            }
        }
    }
    else
    {
        if(const auto it = _signalsSlotsRegistery.find(signalToCheck) ; it != _signalsSlotsRegistery.cend())
        {
            for(const auto& tuple : it->second)
            {
                if(asSlot == std::get<AsSlotPtr_Index>(tuple) && std::find(_funcPtrToSlot<FunctionPtr_t>.cbegin() , _funcPtrToSlot<FunctionPtr_t>.cend() , std::get<SlotObjPtr_Index>(tuple)) != _funcPtrToSlot<FunctionPtr_t>.cend())
                {
                    connections.emplace(signalToCheck , tuple);
                }
            }
        }
    }

    return connections;
}

template<typename FunctionPtr_t>
inline bool SignalsSlotsManager::checkIfSameConnectionExists(SignalPtr signalToCheck, SlotObjPtr slotToCheck, AsSlot* asSlot, FunctionPtr_t func) const
{
    if(asSlot == nullptr)
    {
        if(const auto it = _signalsSlotsRegistery.find(signalToCheck) ; it != _signalsSlotsRegistery.cend())
        {
            for(const auto& tuple : it->second)
            {
                if(slotToCheck == std::get<SlotObjPtr_Index>(tuple))
                {
                    return true;
                }
            }
        }
    }
    else
    {
        if(const auto it = _signalsSlotsRegistery.find(signalToCheck) ; it != _signalsSlotsRegistery.cend())
        {
            for(const auto& tuple : it->second)
            {
                if(asSlot == std::get<AsSlotPtr_Index>(tuple) && std::find(_funcPtrToSlot<FunctionPtr_t>.cbegin() , _funcPtrToSlot<FunctionPtr_t>.cend() , std::get<SlotObjPtr_Index>(tuple)) != _funcPtrToSlot<FunctionPtr_t>.cend())
                {
                    return true;
                }
            }
        }
    }

    return false;
}

template<typename FunctionPtr_t>
inline void SignalsSlotsManager::connectSignalSlot(SignalPtr signal, SlotObjPtr slotObj, AsSlot* asSlot, FunctionPtr_t func, SlotType slotType, Type connectionType)
{
    if(connectionType == Single && checkIfSameConnectionExists(signal , slotObj , asSlot , func))
    {
        return;
    }

    if(func != nullptr)
    {
        std::lock_guard lock(_mutex);

        _funcPtrToSlot<FunctionPtr_t>.emplace_back(slotObj);
    }

    std::lock_guard lock(_mutex);

    _signalsSlotsRegistery[signal].emplace_back(std::tuple(slotObj , asSlot , slotType , connectionType));
}

template<typename FunctionPtr_t>
inline void SignalsSlotsManager::disconnectSignalSlot(SignalPtr signal, SlotObjPtr slot, AsSlot* asSlot, FunctionPtr_t func, Type type)
{
    if(asSlot == nullptr)
    {
        if(const auto it = _signalsSlotsRegistery.find(signal) ; it != _signalsSlotsRegistery.cend())
        {
            std::lock_guard lock(_mutex);

            for(auto it2 = it->second.cbegin() ; it2 != it->second.cend();)
            {
                if(std::get<SlotObjPtr_Index>(*it2) == slot)
                {
                    it2 = it->second.erase(it2);

                    if(type == Type::Single)
                    {
                        return;
                    }
                }
                else
                {
                    ++it2;
                }
            }
        }
    }
    else
    {
        if(const auto  it = _signalsSlotsRegistery.find(signal) ; it != _signalsSlotsRegistery.cend())
        {
            for(auto it2 = it->second.cbegin() ; it2 != it->second.cend();)
            {
                if(const auto& tuple = *it2 ; std::get<AsSlotPtr_Index>(tuple) == asSlot)
                {
                    std::lock_guard lock(_mutex);

                    if(const auto it3 = std::find(_funcPtrToSlot<FunctionPtr_t>.cbegin() , _funcPtrToSlot<FunctionPtr_t>.cend() , std::get<SlotObjPtr_Index>(tuple)) ; it3 != _funcPtrToSlot<FunctionPtr_t>.cend())
                    {
                        delete static_cast<SlotBase*>(std::get<SlotObjPtr_Index>(tuple));

                        it2 = it->second.erase(it2);

                        _funcPtrToSlot<FunctionPtr_t>.erase(it3);

                        if(type == Type::Single)
                        {
                            return;
                        }
                    }
                }
                else
                {
                    ++it2;
                }
            }
        }
    }
}

template<typename... Args_t>
inline void SignalsSlotsManager::onSignalEmited(SignalPtr signal, Args_t&&... args)
{
    if(const auto it = _signalsSlotsRegistery.find(signal) ; it != _signalsSlotsRegistery.cend())
    {
        for(const auto& tuple : it->second)
        {
            (*(static_cast<Slot<decltype(args)...>*>(std::get<SlotObjPtr_Index>(tuple))))(std::forward<decltype(args)>(args)...);
        }
    }
}

