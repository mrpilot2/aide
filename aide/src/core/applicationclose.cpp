#include "applicationclose.hpp"

#include <utility>

using aide::core::ApplicationClose;
using aide::core::ApplicationCloseViewWeakPtr;

ApplicationClose::ApplicationClose(ApplicationCloseViewWeakPtr v)
    : view{std::move(v)}
{}

bool ApplicationClose::isCloseAllowed() const
{
    if (not view.expired()) {
        auto ptr = view.lock();

        return ptr->letUserConfirmApplicationClose();
    }
    return false;
}
