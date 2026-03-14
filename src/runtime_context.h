#pragma once
#include "projection_context.h"
#include <stdexcept>

// Singleton that holds a raw pointer to the "current" C++ objects.
// The projection loop sets this pointer before calling into Python
// and clears it afterward.  The pybind11 module exposes free functions
// that read/write through this pointer — so Python is always operating
// on live C++ memory, not a copy.
class RuntimeContext {
public:
    static RuntimeContext& instance() {
        static RuntimeContext ctx;
        return ctx;
    }

    void bind(ProjectionContext* p)   { ctx_ = p; }
    void unbind()                     { ctx_ = nullptr; }

    ProjectionContext& get() {
        if (!ctx_)
            throw std::runtime_error(
                "actuarial: no active projection context — "
                "functions can only be called during a projection loop");
        return *ctx_;
    }

    // RAII guard — binds on construction, unbinds on destruction.
    // Handles exceptions and early returns automatically.
    class Guard {
    public:
        Guard(ProjectionContext& ctx) {
            RuntimeContext::instance().bind(&ctx);
        }
        ~Guard() {
            RuntimeContext::instance().unbind();
        }
        Guard(const Guard&) = delete;
        Guard& operator=(const Guard&) = delete;
    };

private:
    RuntimeContext() = default;
    ProjectionContext* ctx_ = nullptr;
};
