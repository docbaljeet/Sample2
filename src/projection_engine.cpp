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

double ProjectionEngine::simple_qx(int attained_age) {
    return 0.001 * (static_cast<double>(attained_age) / 100.0);
}

void ProjectionEngine::run(int projection_months, ProjectionHook hook) {
    for (const auto& pol : policies_) {
        for (const auto& scn : scenarios_) {

            // Per-policy/scenario state living on the stack.
            ProjectionContext ctx;
            ctx.policy_id         = pol.policy_id;
            ctx.scenario_id       = scn.scenario_id;
            ctx.face_amount       = pol.face_amount;
            ctx.premium           = pol.premium;
            ctx.issue_age         = pol.issue_age;
            ctx.policy_term_years = pol.term_years;
            ctx.interest_rate     = scn.interest_rate;
            ctx.account_value     = 0.0;
            ctx.reserve           = 0.0;

            for (int m = 1; m <= projection_months; ++m) {
                ctx.month = m;

                int attained_age   = pol.issue_age + (m - 1) / 12;
                ctx.mortality_rate = simple_qx(attained_age);

                // ── Simple monthly projection formulas ──────────
                double monthly_rate = ctx.interest_rate / 12.0;
                ctx.account_value   = (ctx.account_value + ctx.premium)
                                      * (1.0 + monthly_rate);
                ctx.death_benefit   = std::max(ctx.face_amount,
                                               ctx.account_value);
                ctx.reserve         = ctx.account_value * 0.95;

                // ── Bind the live context, call the hook, unbind ─
                // While the hook runs, any call to actuarial.get_*()
                // or actuarial.set_*() from Python will read/write
                // this exact `ctx` on the C++ stack.
                if (hook) {
                    RuntimeContext::Guard guard(ctx);
                    hook();
                }
            }

            std::cout << "  Policy " << pol.policy_id
                      << " / Scenario " << scn.scenario_id
                      << " => final AV = " << ctx.account_value
                      << "  reserve = " << ctx.reserve
                      << "\n";
        }
    }
}
