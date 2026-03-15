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
    int    gender;         // 0=male, 1=female
    int    smoker_status;  // 0=non-smoker, 1=smoker
};

struct ScenarioData {
    int    scenario_id;
    double interest_rate;
    double participation_rate;
    double cap_rate;
    double floor_rate;
    double spread;
};

// One hook per script type — each takes no arguments.
// The guard is set before calling, so the script accesses
// its inputs/outputs through the actuarial.* submodule.
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

    // Simulates a monthly equity index return for demo purposes.
    static double simple_index_return(int month);
};
