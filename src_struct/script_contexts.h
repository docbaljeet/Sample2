#pragma once
#include <vector>

// Same typed structs as src_tls_vec / src_codegen.
// These get exposed directly to Python as classes via pybind11.
// def_readonly for inputs, def_readwrite for outputs.

struct MortalityContext {
    // READ-ONLY inputs
    int    projection_months = 0;
    int    issue_age         = 0;
    int    gender            = 0;
    int    smoker_status     = 0;
    double face_amount       = 0.0;

    // READ-WRITE output
    std::vector<double> qx_monthly;
};

struct LapseContext {
    // READ-ONLY inputs
    int    projection_months = 0;
    int    policy_term_years = 0;
    double premium           = 0.0;

    // READ-WRITE output
    std::vector<double> lapse_rate_monthly;
};

struct EiaCreditContext {
    // READ-ONLY inputs
    int    projection_months  = 0;
    double participation_rate = 0.0;
    double cap_rate           = 0.0;
    double floor_rate         = 0.0;
    double spread             = 0.0;
    std::vector<double> index_returns;

    // READ-WRITE output
    std::vector<double> credited_rate;
};
