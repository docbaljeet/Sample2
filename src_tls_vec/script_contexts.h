#pragma once
#include <vector>

// Each script is called ONCE per (policy, scenario) before the month loop.
// Inputs are policy/scenario-level data.
// Outputs are vectors — one value per projection month.
// C++ uses these vectors in the fast inner loop with no Python calls.

// ─────────────────────────────────────────────────────────────────────
struct MortalityContext {
    // READ-ONLY inputs (set by C++ before calling the script)
    int    projection_months = 0;
    int    issue_age         = 0;
    int    gender            = 0;     // 0=male, 1=female
    int    smoker_status     = 0;     // 0=non-smoker, 1=smoker
    double face_amount       = 0.0;

    // WRITE-ONLY outputs (filled by Python — one value per month)
    std::vector<double> qx_monthly;   // monthly mortality rate [0..projection_months-1]
};

// ─────────────────────────────────────────────────────────────────────
struct LapseContext {
    // READ-ONLY inputs
    int    projection_months = 0;
    int    policy_term_years = 0;
    double premium           = 0.0;

    // WRITE-ONLY outputs
    std::vector<double> lapse_rate_monthly; // monthly lapse rate per month
};

// ─────────────────────────────────────────────────────────────────────
struct EIACreditContext {
    // READ-ONLY inputs
    int    projection_months  = 0;
    double participation_rate = 0.0;
    double cap_rate           = 0.0;
    double floor_rate         = 0.0;
    double spread             = 0.0;

    // READ-ONLY input vector — one value per month (C++ provides the scenario)
    std::vector<double> index_returns;

    // WRITE-ONLY outputs
    std::vector<double> credited_rate; // credited rate per month
};
