#pragma once
#include "projection_context.h"
#include <vector>
#include <functional>

// Simple policy record loaded before projection.
struct PolicyData {
    int    policy_id;
    double face_amount;
    double premium;
    int    issue_age;
    int    term_years;
};

// Simple scenario record.
struct ScenarioData {
    int    scenario_id;
    double interest_rate;   // annual
};

// The hook is a no-argument callback.  It doesn't receive the context
// as a parameter — instead it accesses C++ state through the functions
// exposed by the pybind11 module (which read/write via RuntimeContext).
using ProjectionHook = std::function<void()>;

class ProjectionEngine {
public:
    void add_policy(PolicyData p);
    void add_scenario(ScenarioData s);

    // Run the full projection.  Before each hook call the engine binds
    // the current ProjectionContext into the RuntimeContext singleton
    // so that the pybind11-exposed functions operate on live data.
    void run(int projection_months, ProjectionHook hook);

private:
    std::vector<PolicyData>   policies_;
    std::vector<ScenarioData> scenarios_;

    static double simple_qx(int attained_age);
};
