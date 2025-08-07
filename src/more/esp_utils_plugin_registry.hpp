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
#include <functional>
#include <vector>
#include <algorithm>
#include <iterator>
#ifdef __GNUG__
#   include <cxxabi.h>
#endif

namespace esp_utils {

/**
 * @brief Plugin information structure for unified registry
 */
template <typename T>
struct PluginInfo {
    std::string name;                    ///< Plugin name
    std::type_index type_idx;           ///< Type index
    std::shared_ptr<T> instance;        ///< Plugin instance (may be null if not created yet)
    std::function<std::shared_ptr<T>()> factory; ///< Factory function to create instances
    std::string type_name;              ///< Real type name (demangled)

    PluginInfo(const std::string &n, std::type_index t, std::function<std::shared_ptr<T>()> f, const std::string &tn)
        : name(n), type_idx(t), instance(nullptr), factory(std::move(f)), type_name(tn) {}

    PluginInfo(const std::string &n, std::type_index t, std::shared_ptr<T> inst, const std::string &tn)
        : name(n), type_idx(t), instance(inst), factory(nullptr), type_name(tn) {}

    std::string dump() const
    {
        return "Plugin Info:"
               "\n\t- Name: " + name +
               "\n\t- Type: " + type_name +
               "\n\t- Has Factory: " + (hasFactory() ? "Yes" : "No") +
               "\n\t- Has Instance: " + (hasInstance() ? "Yes" : "No");
    }

    bool hasFactory() const
    {
        return static_cast<bool>(factory);
    }

    bool hasInstance() const
    {
        return static_cast<bool>(instance);
    }
};

/**
 * @brief Plugin registration and management class
 * Uses unified vector-based storage for all plugin information
 *
 * @tparam T Base class type for plugins
 */
template <typename T>
class PluginRegistry {
public:
    using InstancePtr = std::shared_ptr<T>;
    using FactoryFunc = std::function<InstancePtr()>;
    using PluginInfoType = PluginInfo<T>;

private:
    static std::vector<PluginInfoType> &getPlugins()
    {
        static std::vector<PluginInfoType> plugins;
        return plugins;
    }

    static std::recursive_mutex &getMutex()
    {
        static std::recursive_mutex mtx;
        return mtx;
    }

public:
    /**
     * @brief Simple iterator for plugins
     */
    using iterator = typename std::vector<PluginInfoType>::iterator;
    using const_iterator = typename std::vector<PluginInfoType>::const_iterator;

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

        auto type_key = std::type_index(typeid(PluginType));
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        auto &plugins = getPlugins();

        for (auto &plugin : plugins) {
            if (plugin.type_idx == type_key) {
                if (!plugin.instance && plugin.factory) {
                    plugin.instance = plugin.factory();
                }
                return std::static_pointer_cast<PluginType>(plugin.instance);
            }
        }
        return nullptr;
    }

    /**
     * @brief Get instance by registered name (returns first match)
     *
     * @param[in] name Plugin name to get
     * @return Shared pointer to the plugin instance (first match if multiple exist)
     */
    static InstancePtr get(const std::string &name)
    {
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        auto &plugins = getPlugins();

        for (auto &plugin : plugins) {
            if (plugin.name == name) {
                if (!plugin.instance && plugin.factory) {
                    plugin.instance = plugin.factory();
                }
                return plugin.instance;
            }
        }
        return nullptr;
    }

    /**
     * @brief Get all instances by registered name
     *
     * @param[in] name Plugin name to get all instances for
     * @return Vector of shared pointers to all matching plugin instances
     */
    static std::vector<InstancePtr> getAll(const std::string &name)
    {
        std::vector<InstancePtr> instances;
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        auto &plugins = getPlugins();

        for (auto &plugin : plugins) {
            if (plugin.name == name) {
                if (!plugin.instance && plugin.factory) {
                    plugin.instance = plugin.factory();
                }
                if (plugin.instance) {
                    instances.push_back(plugin.instance);
                }
            }
        }
        return instances;
    }

    /**
     * @brief Get list of all registered plugin names (unique names only)
     *
     * @return Vector containing all registered plugin names
     */
    static std::vector<std::string> listRegisteredNames()
    {
        std::vector<std::string> names;
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        auto &plugins = getPlugins();

        for (const auto &plugin : plugins) {
            if (std::find(names.begin(), names.end(), plugin.name) == names.end()) {
                names.push_back(plugin.name);
            }
        }
        return names;
    }

    /**
     * @brief Get count of plugins registered with each name
     *
     * @return Unordered map with plugin names as keys and count as values
     */
    static std::unordered_map<std::string, size_t> getNameCounts()
    {
        std::unordered_map<std::string, size_t> counts;
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        auto &plugins = getPlugins();

        for (const auto &plugin : plugins) {
            counts[plugin.name]++;
        }
        return counts;
    }

    /**
     * @brief Get iterator to the beginning of all plugins
     *
     * @return const_iterator pointing to the first plugin
     */
    static const_iterator begin()
    {
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        return getPlugins().begin();
    }

    /**
     * @brief Get iterator to the end of all plugins
     *
     * @return const_iterator pointing past the last plugin
     */
    static const_iterator end()
    {
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        return getPlugins().end();
    }

    /**
     * @brief Get all plugin information
     *
     * @return Vector of PluginInfo containing all registered plugins
     */
    static std::vector<PluginInfoType> getAllPluginInfo()
    {
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        return getPlugins();  // Copy the vector
    }

    /**
     * @brief Get plugins by name
     *
     * @param[in] name Plugin name to filter by
     * @return Vector of PluginInfo for plugins with the specified name
     */
    static std::vector<PluginInfoType> getPluginInfoByName(const std::string &name)
    {
        std::vector<PluginInfoType> result;
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        auto &plugins = getPlugins();

        for (const auto &plugin : plugins) {
            if (plugin.name == name) {
                result.push_back(plugin);
            }
        }
        return result;
    }

    /**
     * @brief Execute a function for each registered plugin
     *
     * @param[in] func Function to execute for each plugin (takes PluginInfo as parameter)
     */
    template<typename Func>
    static void forEach(Func func)
    {
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        auto &plugins = getPlugins();
        for (const auto &plugin : plugins) {
            func(plugin);
        }
    }

    /**
     * @brief Execute a function for each plugin with a specific name
     *
     * @param[in] name Plugin name to filter by
     * @param[in] func Function to execute for each matching plugin
     */
    template<typename Func>
    static void forEachByName(const std::string &name, Func func)
    {
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        auto &plugins = getPlugins();
        for (const auto &plugin : plugins) {
            if (plugin.name == name) {
                func(plugin);
            }
        }
    }

    /**
     * @brief Count total number of registered plugins
     *
     * @return Total number of registered plugins
     */
    static size_t getPluginCount()
    {
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        return getPlugins().size();
    }

    /**
     * @brief Find plugin by predicate
     *
     * @param[in] pred Predicate function that returns true for matching plugin
     * @return const_iterator pointing to first matching plugin, or end() if not found
     */
    template<typename Predicate>
    static const_iterator findIf(Predicate pred)
    {
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        auto &plugins = getPlugins();
        return std::find_if(plugins.begin(), plugins.end(), pred);
    }

    /**
     * @brief Remove plugin by name (removes first match only)
     *
     * @param[in] name Plugin name to remove
     * @return true if plugin was found and removed, false otherwise
     */
    static bool removePlugin(const std::string &name)
    {
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        auto &plugins = getPlugins();

        auto it = std::find_if(plugins.begin(), plugins.end(), [&name](const PluginInfoType & plugin) {
            return plugin.name == name;
        });

        if (it != plugins.end()) {
            plugins.erase(it);
            return true;
        }
        return false;
    }

    /**
     * @brief Remove all plugins with specified name
     *
     * @param[in] name Plugin name to remove all instances of
     * @return Number of plugins removed
     */
    static size_t removeAllPlugins(const std::string &name)
    {
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        auto &plugins = getPlugins();

        size_t removed_count = 0;
        auto it = plugins.begin();
        while (it != plugins.end()) {
            if (it->name == name) {
                it = plugins.erase(it);
                ++removed_count;
            } else {
                ++it;
            }
        }
        return removed_count;
    }

    /**
     * @brief Remove plugin by type (removes first match only)
     *
     * @tparam PluginType Specific plugin type to remove
     * @return true if plugin was found and removed, false otherwise
     */
    template <typename PluginType>
    static bool removePluginByType()
    {
        static_assert(std::is_base_of_v<T, PluginType>, "PluginType must inherit from base type T");

        auto type_key = std::type_index(typeid(PluginType));
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        auto &plugins = getPlugins();

        auto it = std::find_if(plugins.begin(), plugins.end(), [&type_key](const PluginInfoType & plugin) {
            return plugin.type_idx == type_key;
        });

        if (it != plugins.end()) {
            plugins.erase(it);
            return true;
        }
        return false;
    }

    /**
     * @brief Remove all plugins of specified type
     *
     * @tparam PluginType Specific plugin type to remove all instances of
     * @return Number of plugins removed
     */
    template <typename PluginType>
    static size_t removeAllPluginsByType()
    {
        static_assert(std::is_base_of_v<T, PluginType>, "PluginType must inherit from base type T");

        auto type_key = std::type_index(typeid(PluginType));
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        auto &plugins = getPlugins();

        size_t removed_count = 0;
        auto it = plugins.begin();
        while (it != plugins.end()) {
            if (it->type_idx == type_key) {
                it = plugins.erase(it);
                ++removed_count;
            } else {
                ++it;
            }
        }
        return removed_count;
    }

    /**
     * @brief Remove specific plugin by name and type
     *
     * @tparam PluginType Specific plugin type to remove
     * @param[in] name Plugin name to remove
     * @return true if plugin was found and removed, false otherwise
     */
    template <typename PluginType>
    static bool removeSpecificPlugin(const std::string &name)
    {
        static_assert(std::is_base_of_v<T, PluginType>, "PluginType must inherit from base type T");

        auto type_key = std::type_index(typeid(PluginType));
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        auto &plugins = getPlugins();

        auto it = std::find_if(plugins.begin(), plugins.end(), [&name, &type_key](const PluginInfoType & plugin) {
            return plugin.name == name && plugin.type_idx == type_key;
        });

        if (it != plugins.end()) {
            plugins.erase(it);
            return true;
        }
        return false;
    }

    /**
     * @brief Remove plugins by predicate
     *
     * @param[in] pred Predicate function that returns true for plugins to remove
     * @return Number of plugins removed
     */
    template<typename Predicate>
    static size_t removeIf(Predicate pred)
    {
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        auto &plugins = getPlugins();

        size_t removed_count = 0;
        auto it = plugins.begin();
        while (it != plugins.end()) {
            if (pred(*it)) {
                it = plugins.erase(it);
                ++removed_count;
            } else {
                ++it;
            }
        }
        return removed_count;
    }

    /**
     * @brief Clear all registered plugins
     *
     * @return Number of plugins removed
     */
    static size_t clearAllPlugins()
    {
        std::lock_guard<std::recursive_mutex> lock(getMutex());
        auto &plugins = getPlugins();
        size_t removed_count = plugins.size();
        plugins.clear();
        return removed_count;
    }

    /**
     * @brief Register a plugin with factory function
     *
     * @tparam PluginType Specific plugin type to register
     * @param[in] name Plugin name (can be duplicated)
     * @param[in] factory Factory function to create instances
     */
    template <typename PluginType>
    static void registerPlugin(const std::string &name, FactoryFunc factory)
    {
        static_assert(std::is_base_of_v<T, PluginType>, "PluginType must inherit from base type T");

        auto type_key = std::type_index(typeid(PluginType));
        std::string type_name = getRealTypeName<PluginType>();

        std::lock_guard<std::recursive_mutex> lock(getMutex());
        auto &plugins = getPlugins();

        // Check if this exact combination already exists
        for (const auto &plugin : plugins) {
            if (plugin.name == name && plugin.type_idx == type_key) {
                return; // Already registered
            }
        }

        plugins.emplace_back(name, type_key, std::move(factory), type_name);
    }

    /**
     * @brief Register a singleton plugin instance
     *
     * @tparam PluginType Specific plugin type to register
     * @param[in] name Plugin name (can be duplicated)
     * @param[in] instance Pre-created singleton instance to register
     */
    template <typename PluginType>
    static void registerSingleton(const std::string &name, std::shared_ptr<PluginType> instance)
    {
        static_assert(std::is_base_of_v<T, PluginType>, "PluginType must inherit from base type T");

        auto type_key = std::type_index(typeid(PluginType));
        std::string type_name = getRealTypeName<PluginType>();

        std::lock_guard<std::recursive_mutex> lock(getMutex());
        auto &plugins = getPlugins();

        // Check if this exact combination already exists
        for (auto &plugin : plugins) {
            if (plugin.name == name && plugin.type_idx == type_key) {
                plugin.instance = std::static_pointer_cast<T>(instance);
                return;
            }
        }

        plugins.emplace_back(name, type_key, std::static_pointer_cast<T>(instance), type_name);
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

    /**
     * @brief Constructor that registers a singleton instance
     *
     * @param[in] name Plugin name
     * @param[in] instance Pre-created singleton instance to register
     */
    PluginRegistrar(const std::string &name, std::shared_ptr<PluginType> instance)
    {
        PluginRegistry<BaseType>::template registerSingleton<PluginType>(name, instance);
    }
};

} // namespace esp_utils

/**
 * @brief Registration macro with default constructor (supports specifying constructor arguments)
 *
 * @param BaseType Base type for the plugin registry
 * @param PluginType Plugin type to register
 * @param name Plugin name
 * @param ... Constructor arguments (optional)
 */
#define ESP_UTILS_REGISTER_PLUGIN_WITH_INSTANCE(BaseType, PluginType, name, instance)         \
    static esp_utils::PluginRegistrar<BaseType, PluginType> _##PluginType##_registrar(name, instance); \

/**
 * @brief Registration macro with custom constructor (supports specifying constructor arguments)
 *
 * @param BaseType Base type for the plugin registry
 * @param PluginType Plugin type to register
 * @param name Plugin name
 * @param creator Custom creator function that returns a shared pointer to the plugin instance
 */
#define ESP_UTILS_REGISTER_PLUGIN_WITH_CONSTRUCTOR(BaseType, PluginType, name, creator)        \
    static esp_utils::PluginRegistrar<BaseType, PluginType> _##PluginType##_registrar(name, creator); \

/**
 * @brief Registration macro (supports specifying constructor arguments) - backward compatibility
 *
 * @param BaseType Base type for the plugin registry
 * @param PluginType Plugin type to register
 * @param name Plugin name
 * @param ... Constructor arguments (optional)
 */
#define ESP_UTILS_REGISTER_PLUGIN(BaseType, PluginType, name, ...)                                  \
    ESP_UTILS_REGISTER_PLUGIN_WITH_CONSTRUCTOR(BaseType, PluginType, name, []() {  \
        return std::make_shared<PluginType>(__VA_ARGS__); \
    })
