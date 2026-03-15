"""
Mortality assumption script (dict marshalling).

inputs:  read-only  (MappingProxyType — TypeError if you try to modify)
outputs: write-only (empty dict — script fills it)
"""


def calc(inputs, outputs):
    months    = inputs["projection_months"]
    issue_age = inputs["issue_age"]
    gender    = inputs["gender"]
    smoker    = inputs["smoker_status"]

    rates = []
    for m in range(months):
        attained_age = issue_age + m // 12

        base_qx = 0.001 * (attained_age / 100.0)
        if gender == 1:
            base_qx *= 0.85
        if smoker == 1:
            base_qx *= 1.50

        rates.append(base_qx / 12.0)

    outputs["qx_monthly"] = rates
