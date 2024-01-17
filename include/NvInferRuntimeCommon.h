/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NV_INFER_RUNTIME_COMMON_H
#define NV_INFER_RUNTIME_COMMON_H

//!
//! \file NvInferRuntimeCommon.h
//!
//! This file provides the nvinfer1::IPluginRegistry interface, which will be moved to the NvInferRuntime.h header
//! in TensorRT 9.0.
//!
//! \warning This file will be removed in TensorRT 9.0.
//!
//! \warning Do not directly include this file. Instead include NvInferRuntime.h
//!
#include "NvInferRuntimeBase.h"
#include "NvInferRuntimePlugin.h"

namespace nvinfer1
{
//!
//! \class IPluginRegistry
//!
//! \brief Single registration point for all plugins in an application. It is
//! used to find plugin implementations during engine deserialization.
//! Internally, the plugin registry is considered to be a singleton so all
//! plugins in an application are part of the same global registry.
//! Note that the plugin registry is only supported for plugins of type
//! IPluginV2 and should also have a corresponding IPluginCreator implementation.
//!
//! \see IPluginV2 and IPluginCreator
//!
//! \warning Do not inherit from this class, as doing so will break forward-compatibility of the API and ABI.
//!
//! \warning In the automotive safety context, be sure to call IPluginRegistry::setErrorRecorder() to register
//! an error recorder with the registry before using other methods in the registry.
//!

class IPluginRegistry
{
public:
    //! Pointer for plugin library handle.
    using PluginLibraryHandle = void*;
    //!
    //! \brief Register a plugin creator. Returns false if one with same type
    //! is already registered.
    //!
    //! \warning The string pluginNamespace must be 1024 bytes or less including the NULL terminator and must be NULL
    //! terminated.
    //!
    //! \usage
    //! - Allowed context for the API call
    //!   - Thread-safe: Yes; calls to this method will be synchronized by a mutex.
    //!
    virtual bool registerCreator(IPluginCreator& creator, AsciiChar const* const pluginNamespace) noexcept = 0;

    //!
    //! \brief Return all the registered plugin creators and the number of
    //! registered plugin creators. Returns nullptr if none found.
    //!
    //! \usage
    //! - Allowed context for the API call
    //!   - Thread-safe: No
    //!
    virtual IPluginCreator* const* getPluginCreatorList(int32_t* const numCreators) const noexcept = 0;

    //!
    //! \brief Return plugin creator based on plugin name, version, and
    //! namespace associated with plugin during network creation.
    //!
    //! \warning The strings pluginName, pluginVersion, and pluginNamespace must be 1024 bytes or less including the
    //! NULL terminator and must be NULL terminated.
    //!
    //! \usage
    //! - Allowed context for the API call
    //!   - Thread-safe: Yes
    //!
    virtual IPluginCreator* getPluginCreator(AsciiChar const* const pluginName, AsciiChar const* const pluginVersion,
        AsciiChar const* const pluginNamespace = "") noexcept
        = 0;

    // @cond SuppressDoxyWarnings
    IPluginRegistry() = default;
    IPluginRegistry(IPluginRegistry const&) = delete;
    IPluginRegistry(IPluginRegistry&&) = delete;
    IPluginRegistry& operator=(IPluginRegistry const&) & = delete;
    IPluginRegistry& operator=(IPluginRegistry&&) & = delete;
// @endcond

protected:
    virtual ~IPluginRegistry() noexcept = default;

public:
    //!
    //! \brief Set the ErrorRecorder for this interface
    //!
    //! Assigns the ErrorRecorder to this interface. The ErrorRecorder will track all errors during execution.
    //! This function will call incRefCount of the registered ErrorRecorder at least once. Setting
    //! recorder to nullptr unregisters the recorder with the interface, resulting in a call to decRefCount if
    //! a recorder has been registered.
    //!
    //! \param recorder The error recorder to register with this interface.
    //
    //! \see getErrorRecorder()
    //!
    //! \usage
    //! - Allowed context for the API call
    //!   - Thread-safe: No
    //!
    virtual void setErrorRecorder(IErrorRecorder* const recorder) noexcept = 0;

    //!
    //! \brief Get the ErrorRecorder assigned to this interface.
    //!
    //! Retrieves the assigned error recorder object for the given class. A default error recorder does not exist,
    //! so a nullptr will be returned if setErrorRecorder has not been called, or an ErrorRecorder has not been
    //! inherited.
    //!
    //! \return A pointer to the IErrorRecorder object that has been registered.
    //!
    //! \see setErrorRecorder()
    //!
    //! \usage
    //! - Allowed context for the API call
    //!   - Thread-safe: Yes
    //!
    virtual IErrorRecorder* getErrorRecorder() const noexcept = 0;

    //!
    //! \brief Deregister a previously registered plugin creator.
    //!
    //! Since there may be a desire to limit the number of plugins,
    //! this function provides a mechanism for removing plugin creators registered in TensorRT.
    //! The plugin creator that is specified by \p creator is removed from TensorRT and no longer tracked.
    //!
    //! \return True if the plugin creator was deregistered, false if it was not found in the registry or otherwise
    //! could
    //!     not be deregistered.
    //!
    //! \usage
    //! - Allowed context for the API call
    //!   - Thread-safe: Yes
    //!
    virtual bool deregisterCreator(IPluginCreator const& creator) noexcept = 0;

    //!
    //! \brief Return whether the parent registry will be searched if a plugin is not found in this registry
    //! default: true
    //!
    //! \return bool variable indicating whether parent search is enabled.
    //!
    //! \see setParentSearchEnabled
    //!
    virtual bool isParentSearchEnabled() const = 0;

    //!
    //! \brief Set whether the parent registry will be searched if a plugin is not found in this registry.
    //!
    //! \param enabled The bool variable indicating whether parent search is enabled.
    //!
    //! \see isParentSearchEnabled
    //!
    virtual void setParentSearchEnabled(bool const enabled) = 0;

    //!
    //! \brief Load and register a shared library of plugins.
    //!
    //! \param pluginPath the plugin library path.
    //!
    //! \return The loaded plugin library handle. The call will fail and return
    //! nullptr if any of the plugins are already registered.
    //!
    virtual PluginLibraryHandle loadLibrary(AsciiChar const* pluginPath) noexcept = 0;

    //!
    //! \brief Deregister plugins associated with a library. Any resources acquired when the library
    //! was loaded will be released.
    //!
    //! \param handle the plugin library handle to deregister.
    //!
    virtual void deregisterLibrary(PluginLibraryHandle handle) noexcept = 0;
};

} // namespace nvinfer1

#endif /* NV_INFER_RUNTIME_COMMON_H */
