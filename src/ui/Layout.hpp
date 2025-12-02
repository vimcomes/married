#pragma once

#include <string>

namespace married::ui {

std::string ellipsize(const std::string& text, int width);
std::string padRight(const std::string& text, int width);

}  // namespace married::ui
