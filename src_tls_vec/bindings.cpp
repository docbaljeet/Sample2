#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include "runtime_context.h"

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(actuarial, m) {
    m.doc() = "Actuarial projection library — vectorized script outputs";

    // ═════════════════════════════════════════════════════════════════
    // actuarial.mortality
    // ═════════════════════════════════════════════════════════════════
    auto mort = m.def_submodule("mortality", "Mortality assumption script");

    // READ-ONLY inputs
    mort.def("get_projection_months", []() { return active_mortality().projection_months; });
    mort.def("get_issue_age",         []() { return active_mortality().issue_age; });
    mort.def("get_gender",            []() { return active_mortality().gender; },
             "0=male, 1=female");
    mort.def("get_smoker_status",     []() { return active_mortality().smoker_status; },
             "0=non-smoker, 1=smoker");
    mort.def("get_face_amount",       []() { return active_mortality().face_amount; });

    // WRITE-ONLY output — full vector for all months
    mort.def("set_qx_monthly",
             [](std::vector<double> v) { active_mortality().qx_monthly = std::move(v); },
             py::arg("rates"),
             "Set monthly mortality rates for all projection months");

    // ═════════════════════════════════════════════════════════════════
    // actuarial.lapse
    // ═════════════════════════════════════════════════════════════════
    auto lapse = m.def_submodule("lapse", "Lapse assumption script");

    // READ-ONLY inputs
    lapse.def("get_projection_months", []() { return active_lapse().projection_months; });
    lapse.def("get_policy_term_years", []() { return active_lapse().policy_term_years; });
    lapse.def("get_premium",           []() { return active_lapse().premium; });

    // WRITE-ONLY output — full vector
    lapse.def("set_lapse_rate_monthly",
              [](std::vector<double> v) { active_lapse().lapse_rate_monthly = std::move(v); },
              py::arg("rates"),
              "Set monthly lapse rates for all projection months");

    // ═════════════════════════════════════════════════════════════════
    // actuarial.eia_credit
    // ═════════════════════════════════════════════════════════════════
    auto eia = m.def_submodule("eia_credit", "EIA credited rate assumption script");

    // READ-ONLY inputs
    eia.def("get_projection_months",  []() { return active_eia_credit().projection_months; });
    eia.def("get_participation_rate", []() { return active_eia_credit().participation_rate; });
    eia.def("get_cap_rate",           []() { return active_eia_credit().cap_rate; });
    eia.def("get_floor_rate",         []() { return active_eia_credit().floor_rate; });
    eia.def("get_spread",             []() { return active_eia_credit().spread; });

    // READ-ONLY input vector — scenario index returns per month
    eia.def("get_index_returns",
            []() -> const std::vector<double>& {
                return active_eia_credit().index_returns;
            },
            py::return_value_policy::reference,
            "Monthly equity index returns (one per month, set by C++)");

    // WRITE-ONLY output — full vector
    eia.def("set_credited_rate",
            [](std::vector<double> v) { active_eia_credit().credited_rate = std::move(v); },
            py::arg("rates"),
            "Set credited rates for all projection months");
}
