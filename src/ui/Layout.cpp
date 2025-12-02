#include "ui/Layout.hpp"

namespace married::ui {

std::string ellipsize(const std::string& text, int width) {
    if (width <= 0) return "";
    if (static_cast<int>(text.size()) <= width) return text;
    if (width <= 3) return text.substr(0, static_cast<std::size_t>(width));
    return text.substr(0, static_cast<std::size_t>(width - 3)) + "...";
}

std::string padRight(const std::string& text, int width) {
    std::string clipped = ellipsize(text, width);
    if (static_cast<int>(clipped.size()) >= width) return clipped;
    return clipped + std::string(static_cast<std::size_t>(width - clipped.size()), ' ');
}

}  // namespace married::ui
