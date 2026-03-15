#pragma once
#include <pybind11/pytypes.h>
#include <functional>
#include <string>
#include <vector>

namespace py = pybind11;

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

// ── Script hooks — hold the Python callables directly ─────────────────
// Each callable takes a context struct reference: calc(ctx)

struct ScriptHooks {
    py::object mortality;     // calc(MortalityContext)
    py::object lapse;         // calc(LapseContext)
    py::object eia_credit;    // calc(EiaCreditContext)
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
