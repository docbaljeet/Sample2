"""
Lapse assumption script (struct passing).

Receives a C++ LapseContext object. Reads inputs as attributes,
writes output directly to the struct.
"""


def calc(ctx):
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
