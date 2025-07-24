/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <string>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <functional>
#include <vector>
#ifdef __GNUG__
#   include <cxxabi.h>
#endif

namespace esp_utils {

/**
 * @brief Plugin registration and management class
 *
 * @tparam T Base class type for plugins
 */
template <typename T>
class PluginRegistry {
public:
    using InstancePtr = std::shared_ptr<T>;
    using FactoryFunc = std::function<InstancePtr()>;

    /**
     * @brief Get instance by type (using typeid)
     *
     * @tparam PluginType Specific plugin type to get
     * @return Shared pointer to the plugin instance
     */
    template <typename PluginType>
    static std::shared_ptr<PluginType> get()
    {
        static_assert(std::is_base_of_v<T, PluginType>, "PluginType must inherit from base type T");

        auto key = std::type_index(typeid(PluginType));
        std::lock_guard<std::mutex> lock(mutex());
        auto it = instanceRegistry().find(key);
        if (it != instanceRegistry().end()) {
            return std::static_pointer_cast<PluginType>(it->second);
        }

        auto it2 = factoryRegistry().find(key);
        if (it2 == factoryRegistry().end()) {
            return nullptr;
        }

        auto instance = it2->second();
        instanceRegistry()[key] = instance;
        return std::static_pointer_cast<PluginType>(instance);
    }

    /**
     * @brief Get instance by registered name
     *
     * @param[in] name Plugin name to get
     * @return Shared pointer to the plugin instance
     */
    static InstancePtr get(const std::string &name)
    {
        std::lock_guard<std::mutex> lock(mutex());
        auto it = nameToType().find(name);
        if (it == nameToType().end()) {
            return nullptr;
        }
        return get(it->second);
    }

    /**
     * @brief Get list of all registered plugin names
     *
     * @return Vector containing all registered plugin names
     */
    static std::vector<std::string> listRegisteredNames()
    {
        std::vector<std::string> names;
        std::lock_guard<std::mutex> lock(mutex());
        for (const auto& [name, _] : nameToType()) {
            names.push_back(name);
        }
        return names;
    }

    /**
     * @brief Get detailed registration information (name -> real type name mapping)
     *
     * @return Unordered map with plugin names as keys and real type names as values
     */
    static std::unordered_map<std::string, std::string> getRegistrationInfo()
    {
        std::lock_guard<std::mutex> lock(mutex());
        std::unordered_map<std::string, std::string> info;
        for (const auto& [name, type_idx] : nameToType()) {
            info[name] = getRealTypeName(type_idx);
        }
        return info;
    }

    /**
     * @brief Register a plugin with factory function
     *
     * @tparam PluginType Specific plugin type to register
     * @param[in] factory Factory function to create instances
     */
    template <typename PluginType>
    static void registerPlugin(const std::string name, FactoryFunc factory)
    {
        static_assert(std::is_base_of_v<T, PluginType>, "PluginType must inherit from base type T");

        auto type_key = std::type_index(typeid(PluginType));
        auto instance = factory();

        std::lock_guard<std::mutex> lock(mutex());
        if (instance != nullptr) {
            instanceRegistry().emplace(type_key, instance);
        }
        factoryRegistry().emplace(type_key, std::move(factory));
        nameToType().emplace(name, type_key);
    }

    /**
     * @brief Get real type name (demangled)
     *
     * @tparam PluginType Type to get name for
     * @return Demangled type name
     */
    template<typename PluginType>
    static std::string getRealTypeName()
    {
        return demangleTypeName(typeid(PluginType).name());
    }

    /**
     * @brief Get real type name from type_index (demangled)
     *
     * @param[in] type_idx Type index to get name for
     * @return Demangled type name
     */
    static std::string getRealTypeName(const std::type_index &type_idx)
    {
        return demangleTypeName(type_idx.name());
    }

private:
    /**
     * @brief Demangle type name for better readability
     *
     * @param[in] mangled_name Mangled type name
     * @return Demangled type name or original name if demangling fails
     */
    static std::string demangleTypeName(const char *mangled_name)
    {
#ifdef __GNUG__
        int status = 0;
        std::unique_ptr<char, void(*)(void *)> result {
            abi::__cxa_demangle(mangled_name, 0, 0, &status),
            std::free
        };
        return (status == 0) ? result.get() : mangled_name;
#else
        // For non-GCC compilers, return original mangled name
        return mangled_name;
#endif
    }

    /**
     * @brief Get factory function registry
     *
     * @return Reference to static factory registry map
     */
    static std::unordered_map<std::type_index, FactoryFunc> &factoryRegistry()
    {
        static std::unordered_map<std::type_index, FactoryFunc> map;
        return map;
    }

    /**
     * @brief Get instance registry
     *
     * @return Reference to static instance registry map
     */
    static std::unordered_map<std::type_index, InstancePtr> &instanceRegistry()
    {
        static std::unordered_map<std::type_index, InstancePtr> map;
        return map;
    }

    /**
     * @brief Get name to type mapping
     *
     * @return Reference to static name to type mapping
     */
    static std::unordered_map<std::string, std::type_index> &nameToType()
    {
        static std::unordered_map<std::string, std::type_index> map;
        return map;
    }

    /**
     * @brief Get mutex for thread safety
     *
     * @return Reference to static mutex
     */
    static std::mutex &mutex()
    {
        static std::mutex mtx;
        return mtx;
    }

    /**
     * @brief Get instance by type_index
     *
     * @param[in] type Type index to get instance for
     * @return Shared pointer to the plugin instance
     */
    static InstancePtr get(std::type_index type)
    {
        auto it = instanceRegistry().find(type);
        if (it != instanceRegistry().end()) {
            return it->second;
        }

        auto fit = factoryRegistry().find(type);
        if (fit == factoryRegistry().end()) {
            return nullptr;
        }

        auto instance = fit->second();
        instanceRegistry()[type] = instance;
        return instance;
    }

protected:
    template <typename BaseType, typename PluginType>
    friend struct PluginRegistrar;
};

/**
 * @brief Registration template - accepts creation function
 *
 * @tparam BaseType Base type for the plugin registry
 * @tparam PluginType Type of plugin to register
 */
template <typename BaseType, typename PluginType>
struct PluginRegistrar {
    /**
     * @brief Constructor that registers the plugin type
     *
     * @param[in] creator Function that creates instances of the plugin
     */
    PluginRegistrar(const std::string &name, std::function<std::shared_ptr<PluginType>()> creator)
    {
        PluginRegistry<BaseType>::template registerPlugin<PluginType>(name, [creator]() {
            return std::static_pointer_cast<BaseType>(creator());
        });
    }
};

} // namespace esp_utils

/**
 * @brief Registration macro (supports specifying constructor arguments)
 *
 * @param BaseType Base type for the plugin registry
 * @param PluginType Plugin type to register
 * @param name Plugin name
 * @param ... Constructor arguments (optional)
 */
#define ESP_UTILS_REGISTER_PLUGIN(BaseType, PluginType, name, ...)                                  \
    static esp_utils::PluginRegistrar<BaseType, PluginType> _##PluginType##_registrar(name, []() {  \
        return std::make_shared<PluginType>(__VA_ARGS__);                                           \
    });
