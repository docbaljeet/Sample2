#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include "runtime_context.h"
#include "actuarial_functions.h"

namespace py = pybind11;

static ProjectionContext& ctx() {
    return RuntimeContext::instance().get();
}

PYBIND11_EMBEDDED_MODULE(actuarial, m) {
    m.doc() = "Actuarial projection library — "
              "functions that operate on live C++ state";

    // ── READ-ONLY inputs (get only — no setters) ────────────────────
    // C++ sets these before calling the hook.  Python reads them
    // as inputs to library functions.

    m.def("get_policy_id",   []() { return ctx().policy_id; },
          "Current policy ID");
    m.def("get_scenario_id", []() { return ctx().scenario_id; },
          "Current scenario ID");
    m.def("get_month",       []() { return ctx().month; },
          "Current projection month (1-based)");
    m.def("get_face_amount", []() { return ctx().face_amount; });
    m.def("get_premium",     []() { return ctx().premium; });
    m.def("get_issue_age",   []() { return ctx().issue_age; });
    m.def("get_policy_term", []() { return ctx().policy_term_years; },
          "Policy term in years");
    m.def("get_interest_rate", []() { return ctx().interest_rate; });
    m.def("get_account_value", []() { return ctx().account_value; });
    m.def("get_death_benefit", []() { return ctx().death_benefit; });
    m.def("get_mortality_rate", []() { return ctx().mortality_rate; });

    // ── WRITE-ONLY outputs (set only — no getters) ──────────────────
    // Python sets these during the hook.  C++ reads them after the
    // hook returns to pick up user-computed results.

    m.def("set_reserve", [](double v) { ctx().reserve = v; },
          py::arg("value"),
          "Set the reserve (output consumed by C++ after hook)");

    m.def("set_user_var",
          [](const std::string& key, double val) {
              ctx().user_vars[key] = val;
          },
          py::arg("key"), py::arg("value"),
          "Write a named user variable (output consumed by C++ after hook)");

    // ── Actuarial computation functions ──────────────────────────────

    m.def("pv_factor",
          [](int n_months) {
              return actuarial_funcs::pv_factor(ctx(), n_months);
          },
          py::arg("n_months"),
          "Present value of $1 discounted for n months at the current rate");

    m.def("net_amount_at_risk",
          []() { return actuarial_funcs::net_amount_at_risk(ctx()); },
          "Death benefit minus reserve (floored at 0)");

    m.def("cost_of_insurance",
          []() { return actuarial_funcs::cost_of_insurance(ctx()); },
          "NAR * monthly mortality rate");

    m.def("annuity_due_factor",
          [](int n_months) {
              return actuarial_funcs::annuity_due_factor(ctx(), n_months);
          },
          py::arg("n_months"),
          "Annuity-due factor for n monthly periods at the current rate");

    m.def("remaining_months",
          []() { return actuarial_funcs::remaining_months(ctx()); },
          "Months left until policy maturity");

    m.def("surrender_value",
          []() { return actuarial_funcs::surrender_value(ctx()); },
          "Account value minus graded surrender charge");
}
