#pragma once

// To-Do
// - Implement assertion for debugging
// - Create an enum for types, then return the style for them
// - Allow for more customisation such as different control heights
// - Allow more customisation in the respective files into the parent renderer:
//   Currently it is rendering everything in the ui_parent.cpp, in handle_relocations.
//   I want to incoporate a handle relocation virtual function into each class then call it dynamically
//   to handle relocations in their respective files which makes much more sense
// - Finish slider implementation
// - Preset color themes
//

#include "ui_style.h"
#include "ui_form.h"
#include "ui_tab.h"
#include "ui_group.h"
#include "ui_checkbox.h"
#include "ui_slider.h"