#pragma once
#include "projection_context.h"
#include <vector>
#include <functional>

struct PolicyData {
    int    policy_id;
    double face_amount;
    double premium;
    int    issue_age;
    int    term_years;
};

struct ScenarioData {
    int    scenario_id;
    double interest_rate;
};

// Hook receives ProjectionContext& — the caller (main) wraps the guard.
using ProjectionHook = std::function<void(ProjectionContext&)>;

class ProjectionEngine {
public:
    void add_policy(PolicyData p);
    void add_scenario(ScenarioData s);
    void run(int projection_months, ProjectionHook hook);

private:
    std::vector<PolicyData>   policies_;
    std::vector<ScenarioData> scenarios_;
    static double simple_qx(int attained_age);
};
