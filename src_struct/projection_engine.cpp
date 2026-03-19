#include "projection_engine.h"
#include "script_contexts.h"
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

void ProjectionEngine::run(int projection_months, const HookRegistry& hooks) {
    for (const auto& pol : policies_) {
        for (const auto& scn : scenarios_) {

            // ── 1. Mortality — pass struct reference to Python ─────
            MortalityContext mort_ctx;
            mort_ctx.projection_months = projection_months;
            mort_ctx.issue_age         = pol.issue_age;
            mort_ctx.gender            = pol.gender;
            mort_ctx.smoker_status     = pol.smoker_status;
            mort_ctx.face_amount       = pol.face_amount;

            hooks.call(Hook::Mortality, mort_ctx);

            // ── 2. Lapse ──────────────────────────────────────────
            LapseContext lapse_ctx;
            lapse_ctx.projection_months = projection_months;
            lapse_ctx.policy_term_years = pol.term_years;
            lapse_ctx.premium           = pol.premium;

            hooks.call(Hook::Lapse, lapse_ctx);

            // ── 3. EIA credited rate ──────────────────────────────
            EiaCreditContext eia_ctx;
            eia_ctx.projection_months  = projection_months;
            eia_ctx.participation_rate = scn.participation_rate;
            eia_ctx.cap_rate           = scn.cap_rate;
            eia_ctx.floor_rate         = scn.floor_rate;
            eia_ctx.spread             = scn.spread;
            eia_ctx.index_returns      = scn.index_returns;

            hooks.call(Hook::EiaCredit, eia_ctx);

            // ── Fast inner loop — pure C++, no Python ─────────────
            // Direct struct field access — same as src_tls_vec.
            double account_value = 0.0;

            for (int m = 0; m < projection_months; ++m) {

                double qx_m    = (m < (int)mort_ctx.qx_monthly.size())
                                 ? mort_ctx.qx_monthly[m] : 0.0;
                double lapse_m = (m < (int)lapse_ctx.lapse_rate_monthly.size())
                                 ? lapse_ctx.lapse_rate_monthly[m] : 0.0;
                double credit  = (m < (int)eia_ctx.credited_rate.size())
                                 ? eia_ctx.credited_rate[m] : 0.0;

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
