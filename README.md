# SignalsSlots: Qt-like Signals and Slots for Modern C++17

> A lightweight, dependency‑free C++17 library that brings the familiar **Qt signals & slots** programming model to native C++ — **no MOC**, no `QObject` inheritance, and no `Q_OBJECT` macro.

---

## Overview

If you have used the Qt framework, you already appreciate how signals and slots elegantly decouple event emitters from handlers. This library reproduces that mechanism with an almost identical developer experience and API surface, while remaining:

* **Pure C++17** — no external dependencies
* **Free from Qt’s meta‑object toolchain** — no code generation required
* **High‑performance** with minimal overhead
* **API‑compatible** — ≈99% identical to Qt’s signals & slots API

> **Important:** This library does **not** include an event loop. When a signal is emitted, all connected slots are executed **immediately** and **synchronously** on the **same thread** that emitted the signal. Events are **not queued**; the invocation is direct.

---

## Features

* Familiar `connect` / `disconnect` API — easy to port Qt code
* No macros, no code generation, no `QObject` inheritance required
* Deterministic behaviour — slots are invoked right when the signal is emitted
* Full support for move semantics (unlike Qt’s implementation)
* Slot callbacks can be replaced at runtime
* Significantly faster than Qt’s signals & slots implementation

---

## Building, Installing, and Uninstalling

### Linux

1. Clone the repository.
2. Create a `build` directory at the project root.
3. Open a terminal in the `build` directory.
4. Configure the build:

   * **Static library**  
     ```bash
     cmake -DCMAKE_BUILD_TYPE=Release ..
     ```
   * **Shared library**  
     ```bash
     cmake -DSIGNALS_SLOTS_BUILD_SHARED=ON -DCMAKE_BUILD_TYPE=Release ..
     ```
   * For a **Debug** build, replace `Release` with `Debug`.

5. Build the library:
   ```bash
   cmake --build .
   ```

6. Install:
   ```bash
   sudo cmake --install .
   ```

7. Uninstall (from the `build` directory):
   ```bash
   sudo cmake -P cmake_uninstall.cmake
   ```

### Windows

1. Clone the repository.
2. Create a `build` directory at the project root.
3. Open the **x64 Native Tools Command Prompt for Visual Studio** **as Administrator** and navigate to the `build` folder.

**If using Visual Studio generators:**

* **Static build**  
  ```powershell
  cmake -S .. -B .
  ```
* **Shared build**  
  ```powershell
  cmake -DSIGNALS_SLOTS_BUILD_SHARED=ON -S .. -B .
  ```
* Build (Release):
  ```powershell
  cmake --build . --config Release
  ```

**If using Ninja or NMake:**

* **Static build**  
  ```powershell
  cmake -DCMAKE_BUILD_TYPE=Release ..
  ```
* **Shared build**  
  ```powershell
  cmake -DSIGNALS_SLOTS_BUILD_SHARED=ON -DCMAKE_BUILD_TYPE=Release ..
  ```
* Build:
  ```powershell
  cmake --build .
  ```
* For **Debug**, replace `Release` with `Debug`.

4. Install:
   ```powershell
   cmake --install .
   ```

5. Uninstall (run as Administrator in the `build` directory):
   ```powershell
   cmake -P cmake_uninstall.cmake
   ```

### macOS

The steps are conceptually the same as for Linux and Windows: create a `build` directory, configure with CMake, build, install, and use the provided uninstall script to remove installed files.

---

## Linking and Importing into Your Project

After installation, add the following to your `CMakeLists.txt`:

```cmake
find_package(SignalsSlots REQUIRED)
target_link_libraries(YourTarget PUBLIC SignalsSlots::SignalsSlots)
```

---

## Examples

### Example 1: Signals and slots with custom resource class

```cpp
#include <iostream>
#include <SignalsSlots/SignalsSlots.h>

// A simple resource class that manages a dynamically allocated integer.
class Resource
{
public:
    Resource(int a)
    {
        _a = new int(a);
    }

    Resource(const Resource& other)
    {
        _a = new int(*other._a);
    }

    Resource(Resource&& other) noexcept
    {
        _a = other._a;
        other._a = nullptr;
    }

    Resource& operator=(const Resource& other)
    {
        if (this != &other)
        {
            delete _a;
            _a = new int(*other._a);
        }
        return *this;
    }

    Resource& operator=(Resource&& other) noexcept
    {
        if (this != &other)
        {
            delete _a;
            _a = other._a;
            other._a = nullptr;
        }
        return *this;
    }

    ~Resource()
    {
        delete _a;
    }

    int getResource() const
    {
        return *_a;
    }

private:
    int* _a;
};

// Receiver1 uses Slot<> instances - in this case, there is no need to inherit from the AsSlot class.
class Receiver1
{
public slots:
    Slot<> slot1{[]()
                 {
                     std::clog << "Receiver1::slot1 executed.\n";
                 }};

    Slot<Resource> slot2{[](Resource r)          // Copies the resource.
                         {
                             std::clog << "Receiver1::slot2 executed. " << r.getResource() << "\n";
                         }};

    Slot<const Resource&> slot3{[](const Resource& r)   // No copy (const reference).
                                 {
                                     std::clog << "Receiver1::slot3 executed. " << r.getResource() << "\n";
                                 }};

    Slot<Resource&> slot4{[](Resource& r)               // No copy (reference).
                           {
                               std::clog << "Receiver1::slot4 executed. " << r.getResource() << "\n";
                           }};

    Slot<Resource&&> slot5{[](Resource&& r)             // Move semantics – no copy.
                            {
                                const auto res = std::move(r);
                                std::clog << "Receiver1::slot5 executed. " << res.getResource() << "\n";
                            }};
};

// Receiver2 uses ordinary member functions – must inherit from AsSlot.
class Receiver2 : public AsSlot
{
public slots:
    void slot1()
    {
        std::clog << "Receiver2::slot1 executed.\n";
    }

    void slot2(Resource r)                     // Copies the resource.
    {
        std::clog << "Receiver2::slot2 executed. " << r.getResource() << "\n";
    }

    void slot3(const Resource& r)              // No copy (const reference).
    {
        std::clog << "Receiver2::slot3 executed. " << r.getResource() << "\n";
    }

    void slot4(Resource& r)                    // No copy (reference).
    {
        std::clog << "Receiver2::slot4 executed. " << r.getResource() << "\n";
    }

    void slot5(Resource&& r)                   // Move semantics – no copy.
    {
        const auto res = std::move(r);
        std::clog << "Receiver2::slot5 executed. " << res.getResource() << "\n";
    }
};

// Sender with several signals – No base class needed for signals.
class Sender
{
public signals:
    Signal<>                   signal1;
    Signal<Resource>           signal2;
    Signal<const Resource&>    signal3;
    Signal<Resource&>          signal4;
    Signal<Resource&&>         signal5;
};

int main()
{
    Sender    sender;
    Receiver1 receiver1;
    Receiver2 receiver2;

    // Connect signals to slots.
    // Note: SignalsSlots::connect is thread‑safe.
    SignalsSlots::connect(&sender, &Sender::signal1, &receiver1, &Receiver1::slot1);
    SignalsSlots::connect(&sender, &Sender::signal2, &receiver1, &Receiver1::slot2);
    SignalsSlots::connect(&sender, &Sender::signal3, &receiver1, &Receiver1::slot3);
    SignalsSlots::connect(&sender, &Sender::signal4, &receiver1, &Receiver1::slot4);
    SignalsSlots::connect(&sender, &Sender::signal5, &receiver1, &Receiver1::slot5);

    SignalsSlots::connect(&sender, &Sender::signal1, &receiver2, &Receiver2::slot1);
    SignalsSlots::connect(&sender, &Sender::signal2, &receiver2, &Receiver2::slot2);
    SignalsSlots::connect(&sender, &Sender::signal3, &receiver2, &Receiver2::slot3);
    SignalsSlots::connect(&sender, &Sender::signal4, &receiver2, &Receiver2::slot4);
    // SignalsSlots::connect(&sender, &Sender::signal5, &receiver2, &Receiver2::slot5);
    // Connecting Receiver2::slot5 would be unsafe here because signal5 moves its argument
    // into the first connected slot; subsequent slots would receive a moved‑from object.
    // SignalsSlots::connect(&sender, &Sender::signal5, &receiver2, &Receiver2::slot5);

    Resource res(1);

    emit sender.signal1();
    emit sender.signal2(res);            // Copies res.
    emit sender.signal3(res);            // No copy (const ref).
    emit sender.signal4(res);            // No copy (ref).
    emit sender.signal5(std::move(res)); // Move semantics – no copy.

    // Slot callbacks can be changed at runtime.
    receiver1.slot2.setCallback([](Resource r)
                                {
                                    std::clog << "Receiver1::slot2 - updated callback executed. " << r.getResource() << "\n";
                                });

    // res was moved; recreate it.
    res = Resource(5);

    emit sender.signal2(res);

    // Disconnect all connections.
    // Note: SignalsSlots::disconnect is thread‑safe.
    SignalsSlots::disconnect(&sender, &Sender::signal1, &receiver1, &Receiver1::slot1);
    SignalsSlots::disconnect(&sender, &Sender::signal2, &receiver1, &Receiver1::slot2);
    SignalsSlots::disconnect(&sender, &Sender::signal3, &receiver1, &Receiver1::slot3);
    SignalsSlots::disconnect(&sender, &Sender::signal4, &receiver1, &Receiver1::slot4);
    SignalsSlots::disconnect(&sender, &Sender::signal5, &receiver1, &Receiver1::slot5);

    SignalsSlots::disconnect(&sender, &Sender::signal1, &receiver2, &Receiver2::slot1);
    SignalsSlots::disconnect(&sender, &Sender::signal2, &receiver2, &Receiver2::slot2);
    SignalsSlots::disconnect(&sender, &Sender::signal3, &receiver2, &Receiver2::slot3);
    SignalsSlots::disconnect(&sender, &Sender::signal4, &receiver2, &Receiver2::slot4);
    // SignalsSlots::disconnect(&sender, &Sender::signal5, &receiver2, &Receiver2::slot5);
    // Receiver2::slot5 was never connected, so no disconnect needed.

    emit sender.signal1();               // No output – all connections removed.
    emit sender.signal2(res);            //
    emit sender.signal3(res);            //
    emit sender.signal4(res);            //
    emit sender.signal5(std::move(res)); //

    return 0;
}

```

### Example 2: Handling duplicate connections

```cpp
#include <iostream>
#include <SignalsSlots/SignalsSlots.h>

class Receiver1   // No inheritance needed – Slot member object.
{
public slots:
    Slot<> slot{[]()
                {
                    std::clog << "Receiver1::slot executed.\n";
                }};
};

class Receiver2 : public AsSlot   // Inheritance required for member‑function slots.
{
public slots:
    void slot()
    {
        std::clog << "Receiver2::slot executed.\n";
    }
};

class Sender   // No base class needed for signals.
{
public signals:
    Signal<> signal;
};

int main()
{
    Sender    sender;
    Receiver1 receiver1;
    Receiver2 receiver2;

    // Note: SignalsSlots::connect is thread‑safe.

    // If SignalsSlots::Single is passed as the fifth argument, the library checks
    // for an existing identical connection and ignores duplicates – similar to Qt's
    // Qt::UniqueConnection behaviour.
    SignalsSlots::connect(&sender, &Sender::signal, &receiver2, &Receiver2::slot, SignalsSlots::Single); // accepted
    SignalsSlots::connect(&sender, &Sender::signal, &receiver2, &Receiver2::slot, SignalsSlots::Single); // ignored (duplicate)
    SignalsSlots::connect(&sender, &Sender::signal, &receiver2, &Receiver2::slot);                        // accepted (no Single)
    SignalsSlots::connect(&sender, &Sender::signal, &receiver2, &Receiver2::slot, SignalsSlots::Single); // ignored
    SignalsSlots::connect(&sender, &Sender::signal, &receiver2, &Receiver2::slot);                        // accepted

    // Four identical connections for receiver1 (no Single flag, so duplicates allowed).
    SignalsSlots::connect(&sender, &Sender::signal, &receiver1, &Receiver1::slot);
    SignalsSlots::connect(&sender, &Sender::signal, &receiver1, &Receiver1::slot);
    SignalsSlots::connect(&sender, &Sender::signal, &receiver1, &Receiver1::slot);
    SignalsSlots::connect(&sender, &Sender::signal, &receiver1, &Receiver1::slot);

    emit sender.signal();   // Each connection fires; duplicates run multiple times.

    // Disconnect: without Single, all identical connections are removed.
    SignalsSlots::disconnect(&sender, &Sender::signal, &receiver2, &Receiver2::slot);

    // For receiver1, we remove duplicates one by one using Single.
    SignalsSlots::disconnect(&sender, &Sender::signal, &receiver1, &Receiver1::slot, SignalsSlots::Single);
    SignalsSlots::disconnect(&sender, &Sender::signal, &receiver1, &Receiver1::slot, SignalsSlots::Single);
    SignalsSlots::disconnect(&sender, &Sender::signal, &receiver1, &Receiver1::slot, SignalsSlots::Single);
    SignalsSlots::disconnect(&sender, &Sender::signal, &receiver1, &Receiver1::slot, SignalsSlots::Single);

    emit sender.signal();   // No output – all connections gone.

    return 0;
}
```

### Example 3: Multiple arguments

```cpp
#include <iostream>
#include <SignalsSlots/SignalsSlots.h>

class Receiver1   // Slot as member object.
{
public slots:
    Slot<int, const std::string&, double> slot{
        [](int a, const std::string& str, double d)
        {
            std::clog << "Receiver1::slot executed. " << a << " " << str << " " << d << "\n";
        }
    };
};

class Receiver2 : public AsSlot   // Member function slot.
{
public slots:
    void slot(int a, const std::string& str, double d)
    {
        std::clog << "Receiver2::slot executed. " << a << " " << str << " " << d << "\n";
    }
};

class Sender
{
public signals:
    Signal<int, const std::string&, double> signal;
};

int main()
{
    Sender    sender;
    Receiver1 receiver1;
    Receiver2 receiver2;

    // Connect signals to slots (thread‑safe).
    SignalsSlots::connect(&sender, &Sender::signal, &receiver1, &Receiver1::slot);
    SignalsSlots::connect(&sender, &Sender::signal, &receiver2, &Receiver2::slot);

    emit sender.signal(5, "Hello", 5.7);

    return 0;
}
```

### Example 4: Move semantics with rvalue references

```cpp
#include <iostream>
#include <SignalsSlots/SignalsSlots.h>

class Receiver1   // Slot as member object.
{
public slots:
    Slot<std::string&&> slot{[](std::string&& str)    // Move semantics – no copy.
                              {
                                  const auto s = std::move(str);
                                  std::clog << "Receiver1::slot executed. " << s << "\n";
                              }};
};

class Receiver2 : public AsSlot   // Member function slot.
{
public slots:
    void slot(std::string&& str)   // Move semantics – no copy.
    {
        const auto s = std::move(str);
        std::clog << "Receiver2::slot executed. " << s << "\n";
    }
};

class Sender
{
public signals:
    Signal<std::string&&> signal1;
    Signal<std::string&&> signal2;
};

int main()
{
    Sender    sender;
    Receiver1 receiver1;
    Receiver2 receiver2;

    // Connect (thread‑safe).
    SignalsSlots::connect(&sender, &Sender::signal1, &receiver1, &Receiver1::slot);
    SignalsSlots::connect(&sender, &Sender::signal2, &receiver2, &Receiver2::slot);

    std::string str1 = "Hello";
    std::string str2 = "Hello again";

    emit sender.signal1(std::move(str1));   // Moves str1 into the slot.
    emit sender.signal2(std::move(str2));   // Moves str2 into the slot.

    return 0;
}
```

### Example 5: Direct connection between a signal and a slot

```cpp
#include <iostream>
#include <SignalsSlots/SignalsSlots.h>

int main()
{
    Signal<int, const std::string&, double> signal;

    Slot<int, const std::string&, double> slot{[](int a, const std::string& str, double d)
                                                {
                                                    std::clog << "Slot executed. " << a << " " << str << " " << d << "\n";
                                                }};

    // Connect signal directly to slot (thread‑safe).
    SignalsSlots::connect(&signal, &slot);

    emit signal(5, "Hello", 5.7);

    // Slot callbacks can be changed at runtime.
    slot.setCallback([](int a, const std::string& str, double d)
                     {
                         std::clog << "Slot executed - updated callback. " << str << " " << a << " " << d << "\n";
                     });

    emit signal(5, "Hello Again", 5.7);

    // Disconnect (thread‑safe).
    SignalsSlots::disconnect(&signal, &slot);

    emit signal(5, "Hello Again", 5.7);   // No output – disconnected.

    return 0;
}
```

### Example 6: Connecting a lambda to a signal

```cpp
#include <iostream>
#include <SignalsSlots/SignalsSlots.h>

class Resource
{
public:
    Resource(int a) : _a(new int(a)) {}
    Resource(const Resource& other) : _a(new int(*other._a)) {}
    Resource(Resource&& other) noexcept : _a(other._a) { other._a = nullptr; }
    Resource& operator=(const Resource& other)
    {
        if (this != &other)
        {
            delete _a;
            _a = new int(*other._a);
        }
        return *this;
    }
    Resource& operator=(Resource&& other) noexcept
    {
        if (this != &other)
        {
            delete _a;
            _a = other._a;
            other._a = nullptr;
        }
        return *this;
    }
    ~Resource() { delete _a; }

    int getResource() const { return *_a; }

private:
    int* _a;
};

int main()
{
    Signal<const Resource&> signal;

    // Connect a lambda directly to the signal (thread‑safe).
    SignalsSlots::connect(&signal, [](const Resource& r)
                          {
                              std::clog << "Lambda executed. " << r.getResource() << "\n";
                          });

    Resource res(50);

    emit signal(res);

    return 0;
}
```

### Example 7: Connecting a lambda to a signal via a sender

```cpp
#include <iostream>
#include <SignalsSlots/SignalsSlots.h>

class Resource
{
public:
    Resource(int a) : _a(new int(a)) {}
    Resource(const Resource& other) : _a(new int(*other._a)) {}
    Resource(Resource&& other) noexcept : _a(other._a) { other._a = nullptr; }
    Resource& operator=(const Resource& other)
    {
        if (this != &other)
        {
            delete _a;
            _a = new int(*other._a);
        }
        return *this;
    }
    Resource& operator=(Resource&& other) noexcept
    {
        if (this != &other)
        {
            delete _a;
            _a = other._a;
            other._a = nullptr;
        }
        return *this;
    }
    ~Resource() { delete _a; }

    int getResource() const { return *_a; }

private:
    int* _a;
};

class Sender
{
public signals:
    Signal<const Resource&> signal;
};

int main()
{
    Sender sender;

    // Connect a lambda to a specific signal of a sender object (thread‑safe).
    SignalsSlots::connect(&sender, &Sender::signal, [](const Resource& r)
                          {
                              std::clog << "Lambda executed. " << r.getResource() << "\n";
                          });

    Resource res(10);

    emit sender.signal(res);

    return 0;
}
```

---
