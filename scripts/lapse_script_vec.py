"""
Lapse assumption script (vectorized).

Called ONCE per (policy, scenario).
Reads policy-level inputs, produces a full vector of monthly lapse rates.
"""
from actuarial import lapse


def calc():
    months = lapse.get_projection_months()
    term_years = lapse.get_policy_term_years()

    total_months = term_years * 12
    rates = []
    for m in range(months):
        policy_year = m // 12 + 1

        if policy_year <= 3:
            annual = 0.08
        elif policy_year <= 7:
            annual = 0.05
        else:
            annual = 0.03

        # Higher surrender charge in early years reduces lapse
        elapsed_frac = (m + 1) / total_months if total_months > 0 else 1.0
        sc_pct = 0.10 * max(0.0, 1.0 - elapsed_frac)
        if sc_pct > 0.05:
            annual *= 0.70

        rates.append(annual / 12.0)

    lapse.set_lapse_rate_monthly(rates)
