#include "projection_engine.h"
#include "runtime_context.h"
#include <cmath>
#include <iostream>

void ProjectionEngine::add_policy(PolicyData p) {
    policies_.push_back(p);
}

void ProjectionEngine::add_scenario(ScenarioData s) {
    scenarios_.push_back(s);
}

double ProjectionEngine::simple_index_return(int month) {
    // Fake monthly index return for demo: oscillates around 0.5%/month.
    return 0.005 + 0.003 * std::sin(month * 0.5);
}

void ProjectionEngine::run(int projection_months, const ScriptHooks& hooks) {
    for (const auto& pol : policies_) {
        for (const auto& scn : scenarios_) {

            double account_value = 0.0;

            for (int m = 1; m <= projection_months; ++m) {

                int attained_age = pol.issue_age + (m - 1) / 12;
                int policy_year  = (m - 1) / 12 + 1;

                // ── 1. Call mortality script ─────────────────────
                MortalityContext mort_ctx;
                mort_ctx.issue_age    = pol.issue_age;
                mort_ctx.attained_age = attained_age;
                mort_ctx.month        = m;
                mort_ctx.gender       = pol.gender;
                mort_ctx.smoker_status = pol.smoker_status;
                mort_ctx.face_amount  = pol.face_amount;

                if (hooks.mortality) {
                    MortalityGuard guard(mort_ctx);
                    hooks.mortality();
                }

                // ── 2. Call EIA credited rate script ─────────────
                EIACreditContext eia_ctx;
                eia_ctx.month              = m;
                eia_ctx.index_return       = simple_index_return(m);
                eia_ctx.participation_rate = scn.participation_rate;
                eia_ctx.cap_rate           = scn.cap_rate;
                eia_ctx.floor_rate         = scn.floor_rate;
                eia_ctx.spread             = scn.spread;

                if (hooks.eia_credit) {
                    EIACreditGuard guard(eia_ctx);
                    hooks.eia_credit();
                }

                // ── 3. Apply credited rate to account value ──────
                double credited = eia_ctx.credited_rate;
                account_value = (account_value + pol.premium)
                                * (1.0 + credited / 12.0);

                double death_benefit = std::max(pol.face_amount,
                                                account_value);

                // ── 4. Surrender value for lapse script ──────────
                int total_months = pol.term_years * 12;
                double elapsed_frac = static_cast<double>(m) / total_months;
                double sc_pct = 0.10 * std::max(0.0, 1.0 - elapsed_frac);
                double surrender_value = account_value * (1.0 - sc_pct);

                // ── 5. Call lapse script ─────────────────────────
                LapseContext lapse_ctx;
                lapse_ctx.month               = m;
                lapse_ctx.policy_year         = policy_year;
                lapse_ctx.premium             = pol.premium;
                lapse_ctx.account_value       = account_value;
                lapse_ctx.surrender_value     = surrender_value;
                lapse_ctx.surrender_charge_pct = sc_pct;

                if (hooks.lapse) {
                    LapseGuard guard(lapse_ctx);
                    hooks.lapse();
                }

                // ── 6. Use script outputs in projection ──────────
                double qx_monthly   = mort_ctx.qx_monthly;
                double lapse_monthly = lapse_ctx.lapse_rate_monthly;

                double death_cost = death_benefit * qx_monthly;
                double lapse_decrement = account_value * lapse_monthly;

                account_value -= (death_cost + lapse_decrement);
                if (account_value < 0.0) account_value = 0.0;

                // Print every 6 months
                if (m % 6 == 0) {
                    std::cout << "  pol=" << pol.policy_id
                              << " scn=" << scn.scenario_id
                              << " month=" << m
                              << "  AV=" << account_value
                              << "  qx_m=" << qx_monthly
                              << "  lapse_m=" << lapse_monthly
                              << "  credited=" << credited
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
