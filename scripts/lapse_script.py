"""
Lapse assumption script.

Inputs (read-only):  actuarial.lapse.get_*()
Outputs (write-only): actuarial.lapse.set_lapse_rate(), set_lapse_rate_monthly()
"""
from actuarial import lapse


def calc():
    year   = lapse.get_policy_year()
    sc_pct = lapse.get_surrender_charge_pct()

    # Higher lapse in early years, drops as surrender charge decreases
    if year <= 3:
        annual_lapse = 0.08
    elif year <= 7:
        annual_lapse = 0.05
    else:
        annual_lapse = 0.03

    # If surrender charge is high, reduce lapse further
    if sc_pct > 0.05:
        annual_lapse *= 0.70

    lapse.set_lapse_rate(annual_lapse)
    lapse.set_lapse_rate_monthly(annual_lapse / 12.0)
