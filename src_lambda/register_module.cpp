#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include "projection_context.h"
#include "actuarial_functions.h"
#include "runtime_context.h"
#include "register_module.h"

namespace py = pybind11;

void register_actuarial_module(ProjectionContext*& active) {
    static py::module_::module_def mod_def;
    auto m = py::module_::create_extension_module(
        "actuarial",
        "Actuarial projection library (lambda-capture approach) — "
        "functions that operate on live C++ state",
        &mod_def);

    // Every lambda captures `active` by reference.  When the projection
    // loop sets `active = &ctx`, all these functions see the live object.

    // ── READ-ONLY inputs (get only — no setters) ────────────────────
    // C++ sets these before calling the hook.  Python reads them
    // as inputs to library functions.

    m.def("get_policy_id",
          [&active]() { return active_ctx(active).policy_id; },
          "Current policy ID");
    m.def("get_scenario_id",
          [&active]() { return active_ctx(active).scenario_id; },
          "Current scenario ID");
    m.def("get_month",
          [&active]() { return active_ctx(active).month; },
          "Current projection month (1-based)");
    m.def("get_face_amount",
          [&active]() { return active_ctx(active).face_amount; });
    m.def("get_premium",
          [&active]() { return active_ctx(active).premium; });
    m.def("get_issue_age",
          [&active]() { return active_ctx(active).issue_age; });
    m.def("get_policy_term",
          [&active]() { return active_ctx(active).policy_term_years; },
          "Policy term in years");
    m.def("get_interest_rate",
          [&active]() { return active_ctx(active).interest_rate; });
    m.def("get_account_value",
          [&active]() { return active_ctx(active).account_value; });
    m.def("get_death_benefit",
          [&active]() { return active_ctx(active).death_benefit; });
    m.def("get_mortality_rate",
          [&active]() { return active_ctx(active).mortality_rate; });

    // ── WRITE-ONLY outputs (set only — no getters) ──────────────────
    // Python sets these during the hook.  C++ reads them after the
    // hook returns to pick up user-computed results.

    m.def("set_reserve",
          [&active](double v) { active_ctx(active).reserve = v; },
          py::arg("value"),
          "Set the reserve (output consumed by C++ after hook)");

    m.def("set_user_var",
          [&active](const std::string& key, double val) {
              active_ctx(active).user_vars[key] = val;
          },
          py::arg("key"), py::arg("value"),
          "Write a named user variable (output consumed by C++ after hook)");

    // ── Actuarial computation functions ──────────────────────────────
    // Pure C++ functions that read the live context.  Python calls
    // these as library functions — the heavy math stays in C++.

    m.def("pv_factor",
          [&active](int n_months) {
              return actuarial_funcs::pv_factor(active_ctx(active), n_months);
          },
          py::arg("n_months"),
          "Present value of $1 discounted for n months at the current rate");

    m.def("net_amount_at_risk",
          [&active]() {
              return actuarial_funcs::net_amount_at_risk(active_ctx(active));
          },
          "Death benefit minus reserve (floored at 0)");

    m.def("cost_of_insurance",
          [&active]() {
              return actuarial_funcs::cost_of_insurance(active_ctx(active));
          },
          "NAR * monthly mortality rate");

    m.def("annuity_due_factor",
          [&active](int n_months) {
              return actuarial_funcs::annuity_due_factor(active_ctx(active), n_months);
          },
          py::arg("n_months"),
          "Annuity-due factor for n monthly periods at the current rate");

    m.def("remaining_months",
          [&active]() {
              return actuarial_funcs::remaining_months(active_ctx(active));
          },
          "Months left until policy maturity");

    m.def("surrender_value",
          [&active]() {
              return actuarial_funcs::surrender_value(active_ctx(active));
          },
          "Account value minus graded surrender charge");

    // ── Inject into sys.modules so `import actuarial` works ─────────
    py::module_::import("sys").attr("modules")["actuarial"] = m;
}
