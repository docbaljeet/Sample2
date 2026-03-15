"""
Mortality assumption script (vectorized).

Called ONCE per (policy, scenario).
Reads policy-level inputs, produces a full vector of monthly qx rates.
"""
from actuarial import mortality


def calc():
    months = mortality.get_projection_months()
    issue_age = mortality.get_issue_age()
    gender = mortality.get_gender()
    smoker = mortality.get_smoker_status()

    rates = []
    for m in range(months):
        attained_age = issue_age + m // 12

        base_qx = 0.001 * (attained_age / 100.0)
        if gender == 1:
            base_qx *= 0.85
        if smoker == 1:
            base_qx *= 1.50

        rates.append(base_qx / 12.0)  # monthly

    mortality.set_qx_monthly(rates)
