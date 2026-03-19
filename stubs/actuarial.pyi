"""
Type stubs for the ``actuarial`` embedded C++ module (struct passing).

Place this file on your IDE's stub search path for autocomplete and
type checking in client assumption scripts.
"""

from typing import Callable, List


# ── Context structs ──────────────────────────────────────────────────

class MortalityContext:
    projection_months: int
    issue_age: int
    gender: int
    smoker_status: int
    face_amount: float
    qx_monthly: List[float]

class LapseContext:
    projection_months: int
    policy_term_years: int
    premium: float
    lapse_rate_monthly: List[float]

class EiaCreditContext:
    projection_months: int
    participation_rate: float
    cap_rate: float
    floor_rate: float
    spread: float
    index_returns: List[float]
    credited_rate: List[float]


# ── Hook decorators ──────────────────────────────────────────────────
# Use as:  @actuarial.mortality
#          def calc(ctx: MortalityContext): ...

class HookDecorator:
    name: str
    func: Callable
    def __call__(self, fn: Callable) -> Callable: ...

mortality: HookDecorator
lapse: HookDecorator
eia_credit: HookDecorator
