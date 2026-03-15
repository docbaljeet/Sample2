#pragma once

// Each script type has its own context struct with clearly separated
// read-only inputs (C++ sets before calling the script) and
// write-only outputs (Python sets, C++ reads after the script returns).

// ─────────────────────────────────────────────────────────────────────
struct MortalityContext {
    // READ-ONLY inputs
    int    issue_age         = 0;
    int    attained_age      = 0;
    int    month             = 0;
    int    gender            = 0;     // 0=male, 1=female
    int    smoker_status     = 0;     // 0=non-smoker, 1=smoker
    double face_amount       = 0.0;

    // WRITE-ONLY outputs
    double qx                = 0.0;   // annual mortality rate
    double qx_monthly        = 0.0;   // monthly mortality rate
};

// ─────────────────────────────────────────────────────────────────────
struct LapseContext {
    // READ-ONLY inputs
    int    month             = 0;
    int    policy_year       = 0;
    double premium           = 0.0;
    double account_value     = 0.0;
    double surrender_value   = 0.0;
    double surrender_charge_pct = 0.0;

    // WRITE-ONLY outputs
    double lapse_rate        = 0.0;   // annual lapse rate
    double lapse_rate_monthly = 0.0;  // monthly lapse rate
};

// ─────────────────────────────────────────────────────────────────────
struct EIACreditContext {
    // READ-ONLY inputs
    int    month             = 0;
    double index_return      = 0.0;   // raw equity index return
    double participation_rate = 0.0;  // e.g. 0.80 = 80%
    double cap_rate          = 0.0;   // e.g. 0.10 = 10% cap
    double floor_rate        = 0.0;   // e.g. 0.00 = 0% floor
    double spread            = 0.0;   // e.g. 0.02 = 2% spread

    // WRITE-ONLY outputs
    double credited_rate     = 0.0;   // final rate credited to account
};
