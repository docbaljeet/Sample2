"""
Mortality assumption script.

Inputs (read-only):  actuarial.mortality.get_*()
Outputs (write-only): actuarial.mortality.set_qx(), set_qx_monthly()
"""
from actuarial import mortality


def calc():
    age    = mortality.get_attained_age()
    gender = mortality.get_gender()
    smoker = mortality.get_smoker_status()

    # Simple qx formula: base rate scales with age, gender, smoking
    base_qx = 0.001 * (age / 100.0)
    if gender == 1:      # female — lower mortality
        base_qx *= 0.85
    if smoker == 1:      # smoker — higher mortality
        base_qx *= 1.50

    mortality.set_qx(base_qx)
    mortality.set_qx_monthly(base_qx / 12.0)
