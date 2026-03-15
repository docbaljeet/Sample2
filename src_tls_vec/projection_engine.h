#pragma once
#include "script_contexts.h"
#include <vector>
#include <functional>

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
    std::vector<double> index_returns; // one per month
};

struct ScriptHooks {
    std::function<void()> mortality;
    std::function<void()> lapse;
    std::function<void()> eia_credit;
};

class ProjectionEngine {
public:
    void add_policy(PolicyData p);
    void add_scenario(ScenarioData s);
    void run(int projection_months, const ScriptHooks& hooks);

private:
    std::vector<PolicyData>   policies_;
    std::vector<ScenarioData> scenarios_;
};
