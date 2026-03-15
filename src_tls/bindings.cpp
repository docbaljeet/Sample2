#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include "runtime_context.h"

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(actuarial, m) {
    m.doc() = "Actuarial projection library — per-script submodules "
              "with read-only inputs and write-only outputs";

    // ═════════════════════════════════════════════════════════════════
    // actuarial.mortality — mortality script inputs/outputs
    // ═════════════════════════════════════════════════════════════════
    auto mort = m.def_submodule("mortality", "Mortality assumption script");

    // READ-ONLY inputs
    mort.def("get_issue_age",    []() { return active_mortality().issue_age; });
    mort.def("get_attained_age", []() { return active_mortality().attained_age; });
    mort.def("get_month",        []() { return active_mortality().month; });
    mort.def("get_gender",       []() { return active_mortality().gender; },
             "0=male, 1=female");
    mort.def("get_smoker_status",[]() { return active_mortality().smoker_status; },
             "0=non-smoker, 1=smoker");
    mort.def("get_face_amount",  []() { return active_mortality().face_amount; });

    // WRITE-ONLY outputs
    mort.def("set_qx",
             [](double v) { active_mortality().qx = v; },
             py::arg("value"), "Annual mortality rate");
    mort.def("set_qx_monthly",
             [](double v) { active_mortality().qx_monthly = v; },
             py::arg("value"), "Monthly mortality rate");

    // ═════════════════════════════════════════════════════════════════
    // actuarial.lapse — lapse script inputs/outputs
    // ═════════════════════════════════════════════════════════════════
    auto lapse = m.def_submodule("lapse", "Lapse assumption script");

    // READ-ONLY inputs
    lapse.def("get_month",        []() { return active_lapse().month; });
    lapse.def("get_policy_year",  []() { return active_lapse().policy_year; });
    lapse.def("get_premium",      []() { return active_lapse().premium; });
    lapse.def("get_account_value",[]() { return active_lapse().account_value; });
    lapse.def("get_surrender_value",
              []() { return active_lapse().surrender_value; });
    lapse.def("get_surrender_charge_pct",
              []() { return active_lapse().surrender_charge_pct; });

    // WRITE-ONLY outputs
    lapse.def("set_lapse_rate",
              [](double v) { active_lapse().lapse_rate = v; },
              py::arg("value"), "Annual lapse rate");
    lapse.def("set_lapse_rate_monthly",
              [](double v) { active_lapse().lapse_rate_monthly = v; },
              py::arg("value"), "Monthly lapse rate");

    // ═════════════════════════════════════════════════════════════════
    // actuarial.eia_credit — EIA credited rate script inputs/outputs
    // ═════════════════════════════════════════════════════════════════
    auto eia = m.def_submodule("eia_credit",
                               "EIA credited rate assumption script");

    // READ-ONLY inputs
    eia.def("get_month",              []() { return active_eia_credit().month; });
    eia.def("get_index_return",       []() { return active_eia_credit().index_return; });
    eia.def("get_participation_rate", []() { return active_eia_credit().participation_rate; });
    eia.def("get_cap_rate",           []() { return active_eia_credit().cap_rate; });
    eia.def("get_floor_rate",         []() { return active_eia_credit().floor_rate; });
    eia.def("get_spread",             []() { return active_eia_credit().spread; });

    // WRITE-ONLY outputs
    eia.def("set_credited_rate",
            [](double v) { active_eia_credit().credited_rate = v; },
            py::arg("value"), "Final credited rate applied to the account");
}
