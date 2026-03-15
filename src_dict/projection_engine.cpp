#include "projection_engine.h"
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <cmath>
#include <iostream>

void ProjectionEngine::add_policy(PolicyData p) {
    policies_.push_back(p);
}

void ProjectionEngine::add_scenario(ScenarioData s) {
    scenarios_.push_back(std::move(s));
}

// Freeze a dict so Python cannot modify it.
// types.MappingProxyType wraps a dict as read-only.
static py::object freeze(py::dict d) {
    static py::object proxy_type =
        py::module_::import("types").attr("MappingProxyType");
    return proxy_type(d);
}

// Create an output dict that only allows writing to predefined keys.
// Any other key raises KeyError with a helpful message.
static py::object make_outputs(const std::vector<std::string>& allowed_keys) {
    static py::object cls =
        py::module_::import("restricted_dict").attr("RestrictedDict");
    py::list keys;
    for (const auto& k : allowed_keys)
        keys.append(k);
    return cls(keys);
}

void ProjectionEngine::run(int projection_months, const ScriptHooks& hooks) {
    for (const auto& pol : policies_) {
        for (const auto& scn : scenarios_) {

            // ── 1. Mortality ──────────────────────────────────────
            // inputs  = frozen read-only proxy
            // outputs = empty mutable dict filled by script
            std::vector<double> qx_monthly;
            if (!hooks.mortality.is_none()) {
                py::dict mort_in;
                mort_in["projection_months"] = projection_months;
                mort_in["issue_age"]         = pol.issue_age;
                mort_in["gender"]            = pol.gender;
                mort_in["smoker_status"]     = pol.smoker_status;
                mort_in["face_amount"]       = pol.face_amount;

                auto mort_out = make_outputs({"qx_monthly"});
                hooks.mortality(freeze(mort_in), mort_out);
                qx_monthly = mort_out.attr("__getitem__")(
                    py::str("qx_monthly")).cast<std::vector<double>>();
            }

            // ── 2. Lapse ──────────────────────────────────────────
            std::vector<double> lapse_monthly;
            if (!hooks.lapse.is_none()) {
                py::dict lapse_in;
                lapse_in["projection_months"]  = projection_months;
                lapse_in["policy_term_years"]  = pol.term_years;
                lapse_in["premium"]            = pol.premium;

                auto lapse_out = make_outputs({"lapse_rate_monthly"});
                hooks.lapse(freeze(lapse_in), lapse_out);
                lapse_monthly = lapse_out.attr("__getitem__")(
                    py::str("lapse_rate_monthly")).cast<std::vector<double>>();
            }

            // ── 3. EIA credited rate ──────────────────────────────
            std::vector<double> credited_rate;
            if (!hooks.eia_credit.is_none()) {
                py::dict eia_in;
                eia_in["projection_months"]  = projection_months;
                eia_in["participation_rate"] = scn.participation_rate;
                eia_in["cap_rate"]           = scn.cap_rate;
                eia_in["floor_rate"]         = scn.floor_rate;
                eia_in["spread"]             = scn.spread;
                eia_in["index_returns"]      = scn.index_returns;

                auto eia_out = make_outputs({"credited_rate"});
                hooks.eia_credit(freeze(eia_in), eia_out);
                credited_rate = eia_out.attr("__getitem__")(
                    py::str("credited_rate")).cast<std::vector<double>>();
            }

            // ── Fast inner loop — pure C++, no Python ─────────────
            double account_value = 0.0;

            for (int m = 0; m < projection_months; ++m) {

                double qx_m    = (m < (int)qx_monthly.size())
                                 ? qx_monthly[m] : 0.0;
                double lapse_m = (m < (int)lapse_monthly.size())
                                 ? lapse_monthly[m] : 0.0;
                double credit  = (m < (int)credited_rate.size())
                                 ? credited_rate[m] : 0.0;

                account_value = (account_value + pol.premium)
                                * (1.0 + credit / 12.0);

                double death_benefit = std::max(pol.face_amount,
                                                 account_value);

                double death_cost      = death_benefit * qx_m;
                double lapse_decrement = account_value * lapse_m;

                account_value -= (death_cost + lapse_decrement);
                if (account_value < 0.0) account_value = 0.0;

                if ((m + 1) % 6 == 0) {
                    std::cout << "  pol=" << pol.policy_id
                              << " scn=" << scn.scenario_id
                              << " month=" << (m + 1)
                              << "  AV=" << account_value
                              << "  qx_m=" << qx_m
                              << "  lapse_m=" << lapse_m
                              << "  credited=" << credit
                              << "\n";
                }
            }

            std::cout << "  Policy " << pol.policy_id
                      << " / Scenario " << scn.scenario_id
                      << " => final AV = " << account_value
                      << "\n";
        }
    }
}
