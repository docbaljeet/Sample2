#pragma once
#include <vector>

// Plain data structs — no macros, no globals.
// Binding metadata lives in bindings.cpp via field descriptors.

struct MortalityContext {
    int    projection_months = 0;
    int    issue_age         = 0;
    int    gender            = 0;
    int    smoker_status     = 0;
    double face_amount       = 0.0;
    std::vector<double> qx_monthly;
};

struct LapseContext {
    int    projection_months = 0;
    int    policy_term_years = 0;
    double premium           = 0.0;
    std::vector<double> lapse_rate_monthly;
};

struct EiaCreditContext {
    int    projection_months  = 0;
    double participation_rate = 0.0;
    double cap_rate           = 0.0;
    double floor_rate         = 0.0;
    double spread             = 0.0;
    std::vector<double> index_returns;
    std::vector<double> credited_rate;
};
