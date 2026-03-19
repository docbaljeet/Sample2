"""
IDE-visible stand-in for the C++ embedded ``actuarial`` module.

At runtime the C++ PYBIND11_EMBEDDED_MODULE takes priority — this file
is never actually executed inside the projection engine.  It exists so
that any Python IDE can resolve ``import actuarial`` and provide
autocomplete, type checking, and hover documentation.
"""

from typing import List


# ── Context structs ──────────────────────────────────────────────────

class MortalityContext:
    """Passed to the @actuarial.mortality hook."""
    projection_months: int
    issue_age: int
    gender: int
    smoker_status: int
    face_amount: float
    qx_monthly: List[float]


class LapseContext:
    """Passed to the @actuarial.lapse hook."""
    projection_months: int
    policy_term_years: int
    premium: float
    lapse_rate_monthly: List[float]


class EiaCreditContext:
    """Passed to the @actuarial.eia_credit hook."""
    projection_months: int
    participation_rate: float
    cap_rate: float
    floor_rate: float
    spread: float
    index_returns: List[float]
    credited_rate: List[float]


# ── Hook decorators ──────────────────────────────────────────────────

class HookDecorator:
    """Decorator that registers a function as a projection hook."""
    name: str
    func: object

    def __call__(self, fn):
        return fn


mortality: HookDecorator = HookDecorator()
"""Register a mortality assumption function."""

lapse: HookDecorator = HookDecorator()
"""Register a lapse assumption function."""

eia_credit: HookDecorator = HookDecorator()
"""Register an EIA crediting assumption function."""
