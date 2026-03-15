#pragma once
#include "script_contexts.h"
#include <stdexcept>

inline thread_local MortalityContext*  g_mortality_ctx  = nullptr;
inline thread_local LapseContext*      g_lapse_ctx      = nullptr;
inline thread_local EIACreditContext*  g_eia_credit_ctx = nullptr;

inline MortalityContext& active_mortality() {
    if (!g_mortality_ctx)
        throw std::runtime_error("mortality script: no active context");
    return *g_mortality_ctx;
}

inline LapseContext& active_lapse() {
    if (!g_lapse_ctx)
        throw std::runtime_error("lapse script: no active context");
    return *g_lapse_ctx;
}

inline EIACreditContext& active_eia_credit() {
    if (!g_eia_credit_ctx)
        throw std::runtime_error("eia_credit script: no active context");
    return *g_eia_credit_ctx;
}

struct MortalityGuard {
    MortalityGuard(MortalityContext& ctx) { g_mortality_ctx = &ctx; }
    ~MortalityGuard() { g_mortality_ctx = nullptr; }
    MortalityGuard(const MortalityGuard&) = delete;
    MortalityGuard& operator=(const MortalityGuard&) = delete;
};

struct LapseGuard {
    LapseGuard(LapseContext& ctx) { g_lapse_ctx = &ctx; }
    ~LapseGuard() { g_lapse_ctx = nullptr; }
    LapseGuard(const LapseGuard&) = delete;
    LapseGuard& operator=(const LapseGuard&) = delete;
};

struct EIACreditGuard {
    EIACreditGuard(EIACreditContext& ctx) { g_eia_credit_ctx = &ctx; }
    ~EIACreditGuard() { g_eia_credit_ctx = nullptr; }
    EIACreditGuard(const EIACreditGuard&) = delete;
    EIACreditGuard& operator=(const EIACreditGuard&) = delete;
};
