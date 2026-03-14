#include "actuarial_functions.h"
#include <cmath>
#include <algorithm>

namespace actuarial_funcs {

double pv_factor(const ProjectionContext& ctx, int n_months) {
    double monthly_rate = ctx.interest_rate / 12.0;
    return std::pow(1.0 + monthly_rate, -n_months);
}

double net_amount_at_risk(const ProjectionContext& ctx) {
    return std::max(0.0, ctx.death_benefit - ctx.reserve);
}

double cost_of_insurance(const ProjectionContext& ctx) {
    double monthly_qx = ctx.mortality_rate / 12.0;
    return net_amount_at_risk(ctx) * monthly_qx;
}

double annuity_due_factor(const ProjectionContext& ctx, int n_months) {
    if (n_months <= 0) return 0.0;
    double monthly_rate = ctx.interest_rate / 12.0;
    if (monthly_rate < 1e-12) return static_cast<double>(n_months);
    double v = 1.0 / (1.0 + monthly_rate);
    // adue = (1 - v^n) / (1 - v)  =  (1 - v^n) / d  where d = 1 - v
    return (1.0 - std::pow(v, n_months)) / (1.0 - v);
}

int remaining_months(const ProjectionContext& ctx) {
    int total = ctx.policy_term_years * 12;
    return std::max(0, total - ctx.month);
}

double surrender_value(const ProjectionContext& ctx) {
    int total_months = ctx.policy_term_years * 12;
    if (total_months <= 0) return ctx.account_value;

    // Surrender charge grades from 10% at month 1 down to 0% at maturity.
    double elapsed_frac = static_cast<double>(ctx.month) / total_months;
    double charge_pct   = 0.10 * std::max(0.0, 1.0 - elapsed_frac);
    return ctx.account_value * (1.0 - charge_pct);
}

} // namespace actuarial_funcs
