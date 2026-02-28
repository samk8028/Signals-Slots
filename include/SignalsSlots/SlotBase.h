#pragma once


class SlotBase
{
public:
    SlotBase(bool heapAllocated) : _heapAllocated(heapAllocated)
    {

    }

    virtual ~SlotBase() = default;


protected:
    inline void setHeapAllocated(bool value) noexcept
    {
        _heapAllocated = value;
    }

    inline bool isHeapAllocated() const noexcept
    {
        return _heapAllocated;
    }


protected:
    bool _heapAllocated;


public:
    friend class SignalsSlots;
    friend class SignalsSlotsManager;
};
