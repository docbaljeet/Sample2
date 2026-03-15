"""
EIA credited rate assumption script (vectorized).

Called ONCE per (policy, scenario).
Reads scenario-level inputs + index return vector,
produces a full vector of credited rates.
"""
from actuarial import eia_credit


def calc():
    months = eia_credit.get_projection_months()
    participation = eia_credit.get_participation_rate()
    cap = eia_credit.get_cap_rate()
    floor = eia_credit.get_floor_rate()
    spread = eia_credit.get_spread()

    index_returns = eia_credit.get_index_returns()

    rates = []
    for m in range(months):
        idx = index_returns[m] if m < len(index_returns) else 0.0

        raw = participation * idx - spread
        credited = max(floor, min(cap, raw))
        rates.append(credited)

    eia_credit.set_credited_rate(rates)
