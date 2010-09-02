// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ppapi/cpp/instance.h"

#include "ppapi/c/dev/ppp_printing_dev.h"
#include "ppapi/c/ppb_instance.h"
#include "ppapi/cpp/dev/scrollbar_dev.h"
#include "ppapi/cpp/dev/widget_dev.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/image_data.h"
#include "ppapi/cpp/logging.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/module_impl.h"
#include "ppapi/cpp/point.h"
#include "ppapi/cpp/resource.h"
#include "ppapi/cpp/var.h"

namespace {

DeviceFuncs<PPB_Instance> ppb_instance_f(PPB_INSTANCE_INTERFACE);

}  // namespace

namespace pp {

Instance::Instance(PP_Instance instance) : pp_instance_(instance) {
}

Instance::~Instance() {
  // Ensure that all per-instance objects have been removed. Generally, these
  // objects should have their lifetime scoped to the instance, such as being
  // instance members or even implemented by your instance sub-class directly.
  //
  // If they're not unregistered at this point, they will usually have a
  // dangling reference to the instance, which can cause a crash later.
  PP_DCHECK(interface_name_to_objects_.empty());
}

bool Instance::Init(uint32_t /*argc*/, const char* /*argn*/[],
                    const char* /*argv*/[]) {
  return true;
}

bool Instance::HandleDocumentLoad(const URLLoader_Dev& /*url_loader*/) {
  return false;
}

bool Instance::HandleEvent(const PP_Event& /*event*/) {
  return false;
}

Var Instance::GetInstanceObject() {
  return Var();
}

void Instance::ViewChanged(const pp::Rect& /*position*/,
                           const pp::Rect& /*clip*/) {
}

Var Instance::GetSelectedText(bool /* html */) {
  return Var();
}

PP_PrintOutputFormat_Dev* Instance::QuerySupportedPrintOutputFormats(
    uint32_t* format_count) {
  *format_count = 0;
  return NULL;
}

int32_t Instance::PrintBegin(const PP_PrintSettings_Dev&) {
  return 0;
}

Resource Instance::PrintPages(
    const PP_PrintPageNumberRange_Dev* /*page_ranges*/,
    uint32_t /*page_range_count*/) {
  return Resource();
}

void Instance::PrintEnd() {
}

void Instance::InvalidateWidget(Widget_Dev /* widget */,
                                const Rect& /* dirty_rect */) {
}

void Instance::ScrollbarValueChanged(Scrollbar_Dev /* scrollbar */,
                                     uint32_t /* value */) {
}

void Instance::Zoom(float /* scale */, bool /* text_only */) {
}

bool Instance::StartFind(const char* /* text */, bool /* case_sensitive */) {
  return false;
}

void Instance::SelectFindResult(bool /* forward */) {
}

void Instance::StopFind() {
}

void Instance::Graphics3DContextLost() {
}

Var Instance::GetWindowObject() {
  if (!ppb_instance_f)
    return Var();
  return Var(Var::PassRef(), ppb_instance_f->GetWindowObject(pp_instance()));
}

Var Instance::GetOwnerElementObject() {
  if (!ppb_instance_f)
    return Var();
  return Var(Var::PassRef(),
             ppb_instance_f->GetOwnerElementObject(pp_instance()));
}

bool Instance::BindGraphics(const Graphics2D& graphics) {
  if (!ppb_instance_f)
    return false;
  return ppb_instance_f->BindGraphics(pp_instance(), graphics.pp_resource());
}

bool Instance::IsFullFrame() {
  if (!ppb_instance_f)
    return false;
  return ppb_instance_f->IsFullFrame(pp_instance());
}

Var Instance::ExecuteScript(const Var& script, Var* exception) {
  if (!ppb_instance_f)
    return Var();
  return Var(Var::PassRef(),
             ppb_instance_f->ExecuteScript(pp_instance(), script.pp_var(),
                                           Var::OutException(exception)));
}

void Instance::AddPerInstanceObject(const std::string& interface_name,
                                    void* object) {
  // Ensure we're not trying to register more than one object per interface
  // type. Otherwise, we'll get confused in GetPerInstanceObject.
  PP_DCHECK(interface_name_to_objects_.find(interface_name) ==
            interface_name_to_objects_.end());
  interface_name_to_objects_[interface_name] = object;
}

void Instance::RemovePerInstanceObject(const std::string& interface_name,
                                       void* object) {
  InterfaceNameToObjectMap::iterator found = interface_name_to_objects_.find(
      interface_name);
  if (found == interface_name_to_objects_.end()) {
    // Attempting to unregister an object that doesn't exist or was already
    // unregistered.
    PP_DCHECK(false);
    return;
  }

  // Validate that we're removing the object we thing we are.
  PP_DCHECK(found->second == object);

  interface_name_to_objects_.erase(found);
}

// static
void* Instance::GetPerInstanceObject(PP_Instance instance,
                                     const std::string& interface_name) {
  Instance* that = Module::Get()->InstanceForPPInstance(instance);
  if (!that)
    return NULL;
  InterfaceNameToObjectMap::iterator found =
      that->interface_name_to_objects_.find(interface_name);
  if (found == that->interface_name_to_objects_.end())
    return NULL;
  return found->second;
}

}  // namespace pp
