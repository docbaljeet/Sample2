#include "projection_engine.h"
#include "script_context.h"
#include "runtime_context.h"
#include <cmath>
#include <iostream>

void ProjectionEngine::add_policy(PolicyData p) {
    policies_.push_back(p);
}

void ProjectionEngine::add_scenario(ScenarioData s) {
    scenarios_.push_back(std::move(s));
}

void ProjectionEngine::run(int projection_months, const ScriptHooks& hooks) {
    for (const auto& pol : policies_) {
        for (const auto& scn : scenarios_) {

            // ── 1. Mortality ──────────────────────────────────────
            ScriptContext mort_ctx;
            mort_ctx.ints["projection_months"] = projection_months;
            mort_ctx.ints["issue_age"]         = pol.issue_age;
            mort_ctx.ints["gender"]            = pol.gender;
            mort_ctx.ints["smoker_status"]     = pol.smoker_status;
            mort_ctx.doubles["face_amount"]    = pol.face_amount;

            if (hooks.mortality) {
                ContextGuard guard("mortality", mort_ctx);
                hooks.mortality();
            }

            // ── 2. Lapse ──────────────────────────────────────────
            ScriptContext lapse_ctx;
            lapse_ctx.ints["projection_months"]  = projection_months;
            lapse_ctx.ints["policy_term_years"]  = pol.term_years;
            lapse_ctx.doubles["premium"]         = pol.premium;

            if (hooks.lapse) {
                ContextGuard guard("lapse", lapse_ctx);
                hooks.lapse();
            }

            // ── 3. EIA credited rate ──────────────────────────────
            ScriptContext eia_ctx;
            eia_ctx.ints["projection_months"]      = projection_months;
            eia_ctx.doubles["participation_rate"]   = scn.participation_rate;
            eia_ctx.doubles["cap_rate"]             = scn.cap_rate;
            eia_ctx.doubles["floor_rate"]           = scn.floor_rate;
            eia_ctx.doubles["spread"]               = scn.spread;
            eia_ctx.vectors["index_returns"]        = scn.index_returns;

            if (hooks.eia_credit) {
                ContextGuard guard("eia_credit", eia_ctx);
                hooks.eia_credit();
            }

            // ── Fast inner loop — pure C++, no Python ─────────────
            // Cache vector references to avoid repeated map lookups
            const auto& qx    = mort_ctx.vectors["qx_monthly"];
            const auto& lapse = lapse_ctx.vectors["lapse_rate_monthly"];
            const auto& credit = eia_ctx.vectors["credited_rate"];

            double account_value = 0.0;

            for (int m = 0; m < projection_months; ++m) {

                double qx_m    = (m < (int)qx.size())     ? qx[m]     : 0.0;
                double lapse_m = (m < (int)lapse.size())   ? lapse[m]  : 0.0;
                double cred_m  = (m < (int)credit.size())  ? credit[m] : 0.0;

                // Apply credited rate
                account_value = (account_value + pol.premium)
                                * (1.0 + cred_m / 12.0);

                double death_benefit = std::max(pol.face_amount,
                                                 account_value);

                // Deduct mortality and lapse costs
                double death_cost      = death_benefit * qx_m;
                double lapse_decrement = account_value * lapse_m;

                account_value -= (death_cost + lapse_decrement);
                if (account_value < 0.0) account_value = 0.0;

                // Print every 6 months
                if ((m + 1) % 6 == 0) {
                    std::cout << "  pol=" << pol.policy_id
                              << " scn=" << scn.scenario_id
                              << " month=" << (m + 1)
                              << "  AV=" << account_value
                              << "  qx_m=" << qx_m
                              << "  lapse_m=" << lapse_m
                              << "  credited=" << cred_m
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
