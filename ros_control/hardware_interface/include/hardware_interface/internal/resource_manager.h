///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012, hiDOF INC.
// Copyright (C) 2013, PAL Robotics S.L.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of hiDOF, Inc. nor the names of its
//     contributors may be used to endorse or promote products derived from
//     this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//////////////////////////////////////////////////////////////////////////////

/// \author Wim Meeussen, Adolfo Rodriguez Tsouroukdissian

#ifndef HARDWARE_INTERFACE_RESOURCE_MANAGER_H
#define HARDWARE_INTERFACE_RESOURCE_MANAGER_H

#include <stdexcept>
#include <string>
#include <map>
#include <vector>
#include <utility>  // for std::make_pair

#include <ros/console.h>

#include <hardware_interface/internal/demangle_symbol.h>

namespace hardware_interface
{

/**
 * \brief Class for handling named resources.
 *
 * Resources are encapsulated inside handle instances, and this class allows to register and get them by name.
 *
 * \tparam ResourceHandle Resource handle type. Must implement the following method:
 *  \code
 *   std::string getName() const;
 *  \endcode
 */
template <class ResourceHandle>
class ResourceManager
{
public:
  /** \name Non Real-Time Safe Functions
   *\{*/

  virtual ~ResourceManager() {}

  /** \return Vector of resource names registered to this interface. */
  std::vector<std::string> getNames() const
  {
    std::vector<std::string> out;
    out.reserve(resource_map_.size());
    for(typename ResourceMap::const_iterator it = resource_map_.begin(); it != resource_map_.end(); ++it)
    {
      out.push_back(it->first);
    }
    return out;
  }

  /**
   * \brief Register a new resource.
   * If the resource name already exists, the previously stored resource value will be replaced with \e val.
   * \param handle Resource value. Its type should implement a <tt>std::string getName()</tt> method.
   */
  void registerHandle(const ResourceHandle& handle)
  {
    typename ResourceMap::iterator it = resource_map_.find(handle.getName());
    if (it == resource_map_.end())
    {
      resource_map_.insert(std::make_pair(handle.getName(), handle));
    }
    else
    {
      ROS_WARN_STREAM("Replacing previously registered handle '" << handle.getName() << "' in '" +
                      internal::demangledTypeName(*this) + "'.");
      it->second = handle;
    }
  }

  /**
   * \brief Get a resource handle by name.
   * \param name Resource name.
   * \return Resource associated to \e name. If the resource name is not found, an exception is thrown.
   */
  ResourceHandle getHandle(const std::string& name)
  {
    typename ResourceMap::const_iterator it = resource_map_.find(name);

    if (it == resource_map_.end())
    {
      ROS_DEBUG_STREAM_NAMED("resource_manager","Available resource handles:");
      for(it = resource_map_.begin(); it != resource_map_.end(); ++it)
      {
        ROS_DEBUG_STREAM_NAMED("resource_manager"," - " << it->first);
      }
      throw std::logic_error("Could not find resource '" + name + "' in '" +
                             internal::demangledTypeName(*this) + "'.");
    }

    return it->second;
  }

  /*\}*/

protected:
  typedef std::map<std::string, ResourceHandle> ResourceMap;
  ResourceMap resource_map_;
};

}

#endif // HARDWARE_INTERFACE_RESOURCE_MANAGER_H
