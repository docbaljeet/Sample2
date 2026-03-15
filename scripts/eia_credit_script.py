"""
EIA credited rate assumption script.

Inputs (read-only):  actuarial.eia_credit.get_*()
Outputs (write-only): actuarial.eia_credit.set_credited_rate()
"""
from actuarial import eia_credit


def calc():
    index_return = eia_credit.get_index_return()
    participation = eia_credit.get_participation_rate()
    cap = eia_credit.get_cap_rate()
    floor = eia_credit.get_floor_rate()
    spread = eia_credit.get_spread()

    # EIA crediting formula:
    # credited = max(floor, min(cap, participation * index_return - spread))
    raw = participation * index_return - spread
    credited = max(floor, min(cap, raw))

    eia_credit.set_credited_rate(credited)
