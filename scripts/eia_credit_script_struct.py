"""
EIA credited rate assumption script (struct passing).

Receives a C++ EiaCreditContext object. Reads inputs as attributes,
writes output directly to the struct.
"""


def calc(ctx):
    rates = []
    for m in range(ctx.projection_months):
        idx = ctx.index_returns[m] if m < len(ctx.index_returns) else 0.0

        raw = ctx.participation_rate * idx - ctx.spread
        credited = max(ctx.floor_rate, min(ctx.cap_rate, raw))
        rates.append(credited)

    ctx.credited_rate = rates
