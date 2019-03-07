#ifndef _RESOURCE_MANAGEMENT_INCLUDE_RESOURCE_MANAGEMENT_RESOURCE_MANAGER_H_
#define _RESOURCE_MANAGEMENT_INCLUDE_RESOURCE_MANAGEMENT_RESOURCE_MANAGER_H_

#include <vector>
#include <string>
#include <utility>

#include <ros/ros.h>

#include "resource_management/ReactiveInputs.h"
#include "resource_management/CoordinationSignals.h"

#include "message_storage/ReactiveBuffer.h"


template<typename ...Types>
struct Impl;

template<typename First, typename ...Types>
struct Impl<First, Types...>
{
    template<typename STLStorage, typename ...Args>
    static void add(STLStorage &storage, Args&... args){
        storage.emplace_back(std::make_shared<ReactiveInputs<First>>(args...));
        Impl<Types...>::add(storage,args...);
    }
};
template<>
struct Impl<>
{
    template<typename STLStorage, typename ...Args>
    static void add(STLStorage &, Args&...){}
};

template<typename CoordinationSignalType, typename ...InputDataTypes>
class ResourceManager
{
public:
    /// also creates 2 buffers for artificial life and coordination signals
    ResourceManager(ros::NodeHandlePtr nh, std::vector<std::string> reactiveInputNames);

protected:
    virtual std::map<std::string,std::shared_ptr<MessageAbstraction>> stateFromMsg(const CoordinationSignalType &msg) = 0;
    virtual std::vector<std::tuple<std::string,std::string,resource_management::EndCondition>>
        transitionFromMsg(const CoordinationSignalType &msg) = 0;
private:

    const std::vector<std::string> &getBufferNames() const;
    void addBufferNames(const std::vector<std::string> &bufferNames);

    /// initializes buffer storage
    /// also clear _reactiveInputs and _reactiveBuffers
    void createReactiveBufferStorage();

    ros::NodeHandlePtr _nh;

    std::shared_ptr<CoordinationSignalsBase> _coordinationSignalService;
    std::vector<std::shared_ptr<ReactiveInputsBase>> _reactiveInputs;

    std::shared_ptr<ReactiveBuffer> _artificialLifeBuffer;
    std::shared_ptr<ReactiveBuffer> _coordinationSignalBuffer;
    std::vector<std::shared_ptr<ReactiveBuffer>> _reactiveBuffers;
    std::shared_ptr<ReactiveBufferStorage> _reactiveBufferStorage;

private:
    std::vector<std::string> _bufferNames;
};


template<typename CoordinationSignalType, typename ...InputDataTypes>
ResourceManager<CoordinationSignalType,InputDataTypes...>::ResourceManager(ros::NodeHandlePtr nh, std::vector<std::string> reactiveInputNames):
    _nh(std::move(nh)), _bufferNames({"artificial_life","coordination_signals"})
{
    this->_coordinationSignalService =
            std::make_shared<CoordinationSignals<CoordinationSignalType>>
                                          (
                                              _nh,
                                              boost::bind(&ResourceManager<CoordinationSignalType,InputDataTypes...>::stateFromMsg,this,_1),
                                              boost::bind(&ResourceManager<CoordinationSignalType,InputDataTypes...>::transitionFromMsg,this,_1)
                                          );
    createReactiveBufferStorage();
    addBufferNames(reactiveInputNames);
    Impl<InputDataTypes...>::add(_reactiveInputs,_nh,reactiveInputNames,*_reactiveBufferStorage);
}

template<typename CoordinationSignalType, typename ...InputDataTypes>
const std::vector<std::string> &ResourceManager<CoordinationSignalType,InputDataTypes...>::getBufferNames() const
{
    return _bufferNames;
}

template<typename CoordinationSignalType, typename ...InputDataTypes>
void ResourceManager<CoordinationSignalType,InputDataTypes...>::addBufferNames(const std::vector<std::string> &bufferNames)
{
    _bufferNames.insert(_bufferNames.end(),bufferNames.begin(),bufferNames.end());
}

template<typename CoordinationSignalType, typename ...InputDataTypes>
void ResourceManager<CoordinationSignalType,InputDataTypes...>::createReactiveBufferStorage()
{
    _reactiveInputs.clear();
    _reactiveBuffers.clear();
    _reactiveBufferStorage=std::make_shared<ReactiveBufferStorage>(getBufferNames());
    _artificialLifeBuffer=_reactiveBufferStorage->operator[]("artificial_life");
    _coordinationSignalBuffer=_reactiveBufferStorage->operator[]("coordination_signals");
}


#endif // _RESOURCE_MANAGEMENT_INCLUDE_RESOURCE_MANAGEMENT_RESOURCE_MANAGER_H_
