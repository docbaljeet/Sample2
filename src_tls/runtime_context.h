#pragma once
#include "projection_context.h"
#include <stdexcept>

// Thread-local pointer to the active projection context.
// No singleton, no class, no instance() call — just a raw pointer
// that each thread owns independently.
inline thread_local ProjectionContext* g_active_ctx = nullptr;

// Fast accessor — one null check, one pointer dereference.
inline ProjectionContext& active_ctx() {
    if (!g_active_ctx)
        throw std::runtime_error(
            "actuarial: no active projection context — "
            "functions can only be called during a projection loop");
    return *g_active_ctx;
}

// RAII guard — sets the pointer on construction, clears on destruction.
struct ContextGuard {
    ContextGuard(ProjectionContext& ctx) { g_active_ctx = &ctx; }
    ~ContextGuard()                     { g_active_ctx = nullptr; }
    ContextGuard(const ContextGuard&) = delete;
    ContextGuard& operator=(const ContextGuard&) = delete;
};
