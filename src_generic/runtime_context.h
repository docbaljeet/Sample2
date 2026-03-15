#pragma once
#include "script_context.h"
#include <stdexcept>
#include <unordered_map>

// ═══════════════════════════════════════════════════════════════════════
// One thread_local map holds the active context for every script.
// One generic guard class works for all scripts.
// No per-script globals, no per-script guard classes.
// ═══════════════════════════════════════════════════════════════════════

inline thread_local
std::unordered_map<std::string, ScriptContext*> g_active_contexts;

// RAII guard — sets the active context on construction, clears on destruction.
struct ContextGuard {
    std::string name_;

    ContextGuard(const std::string& name, ScriptContext& ctx) : name_(name) {
        g_active_contexts[name] = &ctx;
    }
    ~ContextGuard() {
        g_active_contexts[name_] = nullptr;
    }

    ContextGuard(const ContextGuard&) = delete;
    ContextGuard& operator=(const ContextGuard&) = delete;
};

// Retrieve the active context for a given script name.
// Throws if no context is bound (script called outside a guard).
inline ScriptContext& active_context(const std::string& script) {
    auto it = g_active_contexts.find(script);
    if (it == g_active_contexts.end() || !it->second)
        throw std::runtime_error(
            "No active context for script '" + script + "'. "
            "Was the script called outside of a projection run?");
    return *it->second;
}
