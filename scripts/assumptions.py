"""
Client assumption script — all hooks in one file.

Uses actuarial.mortality, actuarial.lapse, actuarial.eia_credit
decorators defined by the C++ engine.
"""

import actuarial


@actuarial.mortality
def calc_mortality(ctx:actuarial.MortalityContext):
    rates = []
    for m in range(ctx.projection_months):
        attained_age = ctx.issue_age + m // 12
        base_qx = 0.001 * (attained_age / 100.0)
        if ctx.gender == 1:
            base_qx *= 0.85
        if ctx.smoker_status == 1:
            base_qx *= 1.50

        rates.append(base_qx / 12.0)

    ctx.qx_monthly = rates


@actuarial.lapse
def calc_lapse(ctx):
    total_months = ctx.policy_term_years * 12
    rates = []
    for m in range(ctx.projection_months):
        policy_year = m // 12 + 1

        if policy_year <= 3:
            annual = 0.08
        elif policy_year <= 7:
            annual = 0.05
        else:
            annual = 0.03

        elapsed_frac = (m + 1) / total_months if total_months > 0 else 1.0
        sc_pct = 0.10 * max(0.0, 1.0 - elapsed_frac)
        if sc_pct > 0.05:
            annual *= 0.70

        rates.append(annual / 12.0)

    ctx.lapse_rate_monthly = rates


@actuarial.eia_credit
def calc_eia_credit(ctx):
    rates = []
    for m in range(ctx.projection_months):
        idx = ctx.index_returns[m] if m < len(ctx.index_returns) else 0.0

        raw = ctx.participation_rate * idx - ctx.spread
        credited = max(ctx.floor_rate, min(ctx.cap_rate, raw))
        rates.append(credited)

    ctx.credited_rate = rates
