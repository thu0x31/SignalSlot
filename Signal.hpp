#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <list>
#include <memory>
#include <ostream>
#include <type_traits>
#include <utility>
#include <concepts>

#ifndef NDEBUG
    #include <iostream>
#endif

namespace thuw {
    template<typename ReturnType, typename ...Args>
    class Signal;

    template<typename ReturnType, typename ...Args>
    class Connection;
}

template<typename ReturnType, typename ...Args>
class thuw::Signal<ReturnType(Args...)> {
public:
    using Slot = std::function<ReturnType(Args...)>;
    std::list<Slot> slotList;

    using Connection = thuw::Connection<ReturnType(Args...)>;

    Signal() = default;
    
    [[nodiscard]] Connection connect(const Slot&& slot) {
        auto&& itr = this->slotList.insert(this->slotList.end(), slot);

        #ifndef NDEBUG
            std::cout << "Signal Slot Size : " << this->slotList.size() << std::endl;
        #endif

        return Connection(this->slotList, itr);
    }

    template<typename ...Args_>
    void operator()(Args_&& ...args) const
    requires std::same_as<ReturnType, void>
    {        
        for(auto& slot : this->slotList) {       
            slot(std::forward<Args_>(args)...);
        }
    }

    template<typename ...Args_>
    [[nodiscard]] std::vector<ReturnType> operator()(Args_&& ...args) const 
    {
        return this->execute<std::vector>(std::forward<Args_>(args)...);
    }

    template<typename Condition, typename ...Args_>
    [[nodiscard]] std::vector<ReturnType> conditionalExecute(Condition conditions, Args_&& ...args) const
    {
        std::vector<ReturnType> vec;

        for (const auto& slot : this->slotList) {            
            ReturnType result = slot(std::forward<Args>(args)...);
            if(conditions(result)) {
                vec.push_back(result);
            };
        }
        
        return std::move(vec);
    }    

    template<template<class, class Allocator=std::allocator<ReturnType>> class Container>
    [[nodiscard]] Container<ReturnType> execute(Args&& ... args) const 
    {
        Container<ReturnType> container(this->slotList.size());
        
        auto&& itr = container.begin();

        for (const auto& slot : this->slotList) {
            *itr = slot(std::forward<Args>(args)...);
            ++itr;
        }
        
        return container;
    }

    bool hasSlot() const {
        return this->slotList.size() > 0;
    }

    //TODO: map
    // template<template<class, class Allocator=std::allocator<ReturnType>> class Container>
    // requires std::same_as<ReturnType, std::pair<ReturnType.firs>>
    // Container<ReturnType> execute(Args& ... args) const {
    //     // std::pair = slot();
    // }
};

template<typename ReturnType, typename ...Args>
class thuw::Connection<ReturnType(Args...)> {
private:
    using Slot = std::function<ReturnType(Args...)>;
    std::list<Slot>* slotList = nullptr;

    using Iterator = typename std::list<Slot>::iterator;
    Iterator iterator;

public:
    Connection() = default;

    Connection(std::list<Slot>& slots, Iterator iterator)
     : slotList(&slots)
     , iterator(iterator){}

    Connection(Connection<ReturnType(Args...)>& connection)
     : iterator(connection.iterator)
    {
        std::swap(this->slotList, connection.slotList);
    }

    Connection(Connection<ReturnType(Args...)>&& connection)
     : iterator(connection.iterator)
    {
        std::swap(this->slotList, connection.slotList);
    }

    template<typename C>
    auto& operator=(C&& connection) {
        this->iterator = connection.iterator;
        std::swap(this->slotList, connection.slotList);

        return *this;
    }

    ~Connection() {
        this->disconnect();
    }

    void disconnect() {
        if(this->slotList == nullptr) {
            return;
        }

        #ifndef NDEBUG
            std::cout << "--------------------" << std::endl;
            std::cout << "Disconnect" << std::endl;
            std::cout << "----------" << std::endl;
            std::cout << "Connection::SlotList Size : " << this->slotList->size() << std::endl;
            std::cout << "----------" << std::endl;
            for(auto itr = this->slotList->begin(); itr != this->slotList->end(); ++itr) {
                std::cout << "Connection::SlotList address : " << &(*itr) << std::endl;
            }
            std::cout << "----------" << std::endl;
            std::cout << "Connection::Iterator address : " << &(*this->iterator) << std::endl;
            std::cout << "----------" << std::endl;
        #endif

        this->slotList->erase(this->iterator);
    }
};

// prototype
// staticSignal.connect([]()->constexpr{});
namespace StaticSignal {

};