#pragma once
#include "projection_context.h"
#include <stdexcept>

// No globals, no singletons, no TLS.
// The pointer lives as a local in main() and is passed around by reference.

// Throws if the pointer is null (called outside a projection loop).
inline ProjectionContext& active_ctx(ProjectionContext* p) {
    if (!p)
        throw std::runtime_error(
            "actuarial: no active projection context — "
            "functions can only be called during a projection loop");
    return *p;
}

// RAII guard — sets the pointer on construction, clears on destruction.
// Takes the pointer by reference so it can modify the caller's local.
struct ContextGuard {
    ProjectionContext*& ptr_;
    ContextGuard(ProjectionContext*& ptr, ProjectionContext& ctx)
        : ptr_(ptr) { ptr_ = &ctx; }
    ~ContextGuard() { ptr_ = nullptr; }
    ContextGuard(const ContextGuard&) = delete;
    ContextGuard& operator=(const ContextGuard&) = delete;
};
