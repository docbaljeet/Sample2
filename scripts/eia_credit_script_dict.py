"""
EIA credited rate assumption script (dict marshalling).

inputs:  read-only  (MappingProxyType — TypeError if you try to modify)
outputs: write-only (empty dict — script fills it)
"""


def calc(inputs, outputs):
    months        = inputs["projection_months"]
    participation = inputs["participation_rate"]
    cap           = inputs["cap_rate"]
    floor         = inputs["floor_rate"]
    spread        = inputs["spread"]
    index_returns = inputs["index_returns"]

    rates = []
    for m in range(months):
        idx = index_returns[m] if m < len(index_returns) else 0.0

        raw = participation * idx - spread
        credited = max(floor, min(cap, raw))
        rates.append(credited)

    outputs["credited_rate"] = rates
