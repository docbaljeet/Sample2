#pragma once
#include <string>
#include <unordered_map>

// Shared state that is visible to both C++ and Python.
// An instance of this struct lives on the C++ stack inside the projection
// loop and is exposed to Python by pointer — no copies.
struct ProjectionContext {

    // ── READ-ONLY from Python (inputs set by C++) ───────────────────
    // Python scripts can read these via get_*() to use as inputs
    // in library functions.  C++ sets them before calling the hook.
    int    policy_id        = 0;
    int    scenario_id      = 0;
    int    month            = 0;         // 1-based projection month
    double face_amount      = 0.0;
    double premium          = 0.0;
    int    issue_age        = 0;
    int    policy_term_years = 0;        // term in years
    double interest_rate    = 0.0;       // annual credited rate
    double account_value    = 0.0;
    double death_benefit    = 0.0;
    double mortality_rate   = 0.0;       // qx for current attained age

    // ── WRITE-ONLY from Python (outputs consumed by C++) ────────────
    // Python scripts set these via set_*().  C++ reads them after
    // the hook returns to pick up user-computed results.
    double reserve          = 0.0;
    std::unordered_map<std::string, double> user_vars;
};
