"""
Mortality assumption script (struct passing).

Receives a C++ MortalityContext object. Reads inputs as attributes,
writes output directly to the struct — no return value needed.
"""


def calc(ctx):
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
