#pragma once

#include <SignalsSlots/AsSlot.h>
#include <SignalsSlots/Signal.h>
#include <SignalsSlots/Slot.h>
#include <SignalsSlots/SignalsSlotsManager.h>


#define signals
#define slots
#define emit


class SignalsSlots
{
private:
    enum SlotType
    {
        SlotObject     = 0 ,
        MemberFunction = 1
    };


public:
    enum Type
    {
        Multi  = 0 ,
        Single = 1
    };


private:
    SignalsSlots() = delete;

    template<typename FunctionPtr_t>
    inline static void privateConnect(SignalPtr signal , SlotObjPtr slotObj , AsSlot* asSlot , FunctionPtr_t func , SlotType slotType , Type connectionType)
    {
        SignalsSlotsManager::getInstance().connectSignalSlot(signal , slotObj , asSlot , func , static_cast<SignalsSlotsManager::SlotType>(slotType) , static_cast<SignalsSlotsManager::Type>(connectionType));
    }

    template<typename FunctionPtr_t>
    inline static void privateDisconnect(SignalPtr signal , SlotObjPtr slot , AsSlot* asSlot , FunctionPtr_t func , Type type)
    {
        SignalsSlotsManager::getInstance().disconnectSignalSlot(signal , slot  , asSlot , func , static_cast<SignalsSlotsManager::Type>(type));
    }


public:
    template<typename SenderObj_t , typename... SignalArgs_t , typename ReceiverObj_t , typename... SlotArgs_t>
    inline static void connect(SenderObj_t* sender , Signal<SignalArgs_t...> SenderObj_t::*signal  , ReceiverObj_t* receiver , Slot<SlotArgs_t...> ReceiverObj_t::*slotObj , Type connectionType = Multi)
    {
        privateConnect(&(sender->*signal) , &(receiver->*slotObj) , nullptr , nullptr , SlotObject , connectionType);
    }

    template<typename SenderObj_t , typename... SignalArgs_t , typename ReceiverObj_t , typename... SlotArgs_t , typename SlotMemberFunctionReturnType>
    inline static void connect(SenderObj_t* sender , Signal<SignalArgs_t...> SenderObj_t::*signal  , ReceiverObj_t* receiver , SlotMemberFunctionReturnType (ReceiverObj_t::*slot)(SlotArgs_t...) , Type connectionType = Multi)
    {
        const auto slotObj = new Slot<SlotArgs_t...>([receiver , slot](SlotArgs_t&&... args){(receiver->*slot)(std::forward<SlotArgs_t>(args)...);});

        slotObj->setHeapAllocated(true);

        privateConnect(&(sender->*signal) , slotObj , receiver , slot , MemberFunction , connectionType);
    }

    template<typename SenderObj_t , typename... SignalArgs_t>
    inline static void connect(SenderObj_t* sender , Signal<SignalArgs_t...> SenderObj_t::*signal  , SlotObjPtr* slotObj , Type connectionType = Multi)
    {
        privateConnect(&(sender->*signal) , slotObj , nullptr , nullptr , SlotObject , connectionType);
    }

    template<typename ReceiverObj_t , typename... SlotArgs_t>
    inline static void connect(SignalPtr signal  , ReceiverObj_t* receiver , Slot<SlotArgs_t...> ReceiverObj_t::*slotObj , Type connectionType = Multi)
    {
        privateConnect(signal , &(receiver->*slotObj) , nullptr , nullptr , SlotObject , connectionType);
    }

    template<typename Signal_t , typename Slot_t>
    inline static void connect(Signal_t signal  , Slot_t* slotObj , Type connectionType = Multi)
    {
        privateConnect(signal , slotObj , nullptr , nullptr , SlotObject , connectionType);
    }

    template<typename Callable_t>
    inline static void connect(SignalPtr signal , Callable_t&& callable)
    {
        const auto slot = new Slot(std::function(std::forward<Callable_t>(callable)));

        slot->setHeapAllocated(true);

        privateConnect(signal , slot , nullptr , nullptr , SlotObject , Multi);
    }

    template<typename SenderObj_t , typename... SignalArgs_t , typename Callable_t>
    inline static void connect(SenderObj_t* sender , Signal<SignalArgs_t...> SenderObj_t::*signal , Callable_t&& callable)
    {
        const auto slot = new Slot(std::function(std::forward<Callable_t>(callable)));

        slot->setHeapAllocated(true);

        privateConnect(&(sender->*signal) , slot , nullptr , nullptr , SlotObject , Multi);
    }

    template<typename SenderObj_t , typename... SignalArgs_t , typename ReceiverObj_t , typename... SlotArgs_t>
    inline static void disconnect(SenderObj_t* sender , Signal<SignalArgs_t...> SenderObj_t::*signal  , ReceiverObj_t* receiver , Slot<SlotArgs_t...> ReceiverObj_t::*slot , Type type = Multi)
    {
        privateDisconnect(&(sender->*signal) , &(receiver->*slot) , nullptr , nullptr , type);
    }

    template<typename SenderObj_t , typename... SignalArgs_t>
    inline static void disconnect(SenderObj_t* sender , Signal<SignalArgs_t...> SenderObj_t::*signal  , SlotObjPtr* slot , Type type = Multi)
    {
        privateDisconnect(&(sender->*signal) , slot , nullptr , nullptr , type);
    }

    template<typename ReceiverObj_t , typename... SlotArgs_t>
    inline static void disconnect(SignalPtr signal  , ReceiverObj_t* receiver , Slot<SlotArgs_t...> ReceiverObj_t::*slot , Type type = Multi)
    {
        privateDisconnect(signal , &(receiver->*slot) , nullptr , nullptr , type);
    }

    template<typename Signal_t , typename Slot_t>
    inline static void disconnect(Signal_t signal  , Slot_t slot , Type type = Multi)
    {
        privateDisconnect(signal , slot , nullptr , nullptr , type);
    }

    template<typename SenderObj_t , typename... SignalArgs_t , typename ReceiverObj_t , typename... SlotArgs_t , typename SlotMemberFunctionReturnType>
    inline static void disconnect(SenderObj_t* sender , Signal<SignalArgs_t...> SenderObj_t::*signal  , ReceiverObj_t* receiver , SlotMemberFunctionReturnType (ReceiverObj_t::*slot)(SlotArgs_t...) , Type type = Multi)
    {
        privateDisconnect(&(sender->*signal) , nullptr , receiver , slot , type);
    }
};
