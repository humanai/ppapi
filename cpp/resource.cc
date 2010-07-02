// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ppapi/cpp/resource.h"

#include <algorithm>

#include "ppapi/cpp/logging.h"
#include "ppapi/cpp/module.h"

namespace pp {

Resource::Resource() : pp_resource_(NULL) {
}

Resource::Resource(const Resource& other) : pp_resource_(other.pp_resource_) {
  if (!is_null())
    Module::Get()->core()->AddRefResource(pp_resource_);
}

Resource::~Resource() {
  if (!is_null())
    Module::Get()->core()->ReleaseResource(pp_resource_);
}

Resource& Resource::operator=(const Resource& other) {
  Resource copy(other);
  swap(copy);
  return *this;
}

void Resource::swap(Resource& other) {
  std::swap(pp_resource_, other.pp_resource_);
}

Resource::Resource(PP_Resource resource) : pp_resource_(resource) {
  if (!is_null())
    Module::Get()->core()->AddRefResource(pp_resource_);
}

void Resource::PassRefFromConstructor(PP_Resource resource) {
  PP_DCHECK(!pp_resource_);
  pp_resource_ = resource;
}

}  // namespace pp
