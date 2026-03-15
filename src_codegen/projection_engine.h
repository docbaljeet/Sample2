#pragma once
#include <functional>
#include <string>
#include <vector>

// ── Policy and scenario data (C++ side, typed) ────────────────────────

struct PolicyData {
    int    policy_id;
    double face_amount;
    double premium;
    int    issue_age;
    int    term_years;
    int    gender;          // 0=male, 1=female
    int    smoker_status;   // 0=non-smoker, 1=smoker
};

struct ScenarioData {
    int    scenario_id;
    double participation_rate;
    double cap_rate;
    double floor_rate;
    double spread;
    std::vector<double> index_returns;
};

// ── Script hooks ──────────────────────────────────────────────────────

struct ScriptHooks {
    std::function<void()> mortality;
    std::function<void()> lapse;
    std::function<void()> eia_credit;
};

// ── Engine ────────────────────────────────────────────────────────────

class ProjectionEngine {
public:
    void add_policy(PolicyData p);
    void add_scenario(ScenarioData s);
    void run(int projection_months, const ScriptHooks& hooks);

private:
    std::vector<PolicyData>   policies_;
    std::vector<ScenarioData> scenarios_;
};
