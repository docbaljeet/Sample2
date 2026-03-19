#pragma once
#include <vector>
#include "hook_registry.h"

// ── Policy and scenario data ──────────────────────────────────────────

struct PolicyData {
    int    policy_id;
    double face_amount;
    double premium;
    int    issue_age;
    int    term_years;
    int    gender;
    int    smoker_status;
};

struct ScenarioData {
    int    scenario_id;
    double participation_rate;
    double cap_rate;
    double floor_rate;
    double spread;
    std::vector<double> index_returns;
};

// ── Engine ────────────────────────────────────────────────────────────

class ProjectionEngine {
public:
    void add_policy(PolicyData p);
    void add_scenario(ScenarioData s);
    void run(int projection_months, const HookRegistry& hooks);

private:
    std::vector<PolicyData>   policies_;
    std::vector<ScenarioData> scenarios_;
};
