"""
Type stubs for the ``actuarial`` embedded C++ module.

This module is compiled into the C++ actuarial projection executable
and is only importable from Python code running inside that process.
All functions operate on live C++ in-memory data structures; they can
only be called during an active projection loop.

Place this file (or the ``stubs/`` directory) on your IDE's stub search
path so you get autocomplete, type checking, and inline documentation.
"""

# ── READ-ONLY inputs (C++ → Python) ─────────────────────────────────
# C++ sets these before calling the hook.  Python reads them as inputs.

def get_policy_id() -> int:
    """Current policy ID."""
    ...

def get_scenario_id() -> int:
    """Current scenario ID."""
    ...

def get_month() -> int:
    """Current projection month (1-based)."""
    ...

def get_face_amount() -> float:
    """Policy face amount."""
    ...

def get_premium() -> float:
    """Monthly premium."""
    ...

def get_issue_age() -> int:
    """Issue age of the insured."""
    ...

def get_policy_term() -> int:
    """Policy term in years."""
    ...

def get_interest_rate() -> float:
    """Annual credited interest rate."""
    ...

def get_account_value() -> float:
    """Current account value."""
    ...

def get_death_benefit() -> float:
    """Current death benefit."""
    ...

def get_mortality_rate() -> float:
    """Mortality rate (qx) for the current attained age."""
    ...

# ── WRITE-ONLY outputs (Python → C++) ───────────────────────────────
# Python sets these during the hook.  C++ reads them after the hook
# returns to pick up user-computed results.

def set_reserve(value: float) -> None:
    """Set the reserve (output consumed by C++ after hook)."""
    ...

def set_user_var(key: str, value: float) -> None:
    """Write a named user variable (output consumed by C++ after hook)."""
    ...

# ── Actuarial computation functions ─────────────────────────────────
# These execute C++ code using the live in-memory projection state.

def pv_factor(n_months: int) -> float:
    """Present value of $1 discounted for *n_months* at the current rate."""
    ...

def net_amount_at_risk() -> float:
    """Death benefit minus reserve, floored at 0."""
    ...

def cost_of_insurance() -> float:
    """Net amount at risk multiplied by the monthly mortality rate."""
    ...

def annuity_due_factor(n_months: int) -> float:
    """Annuity-due factor for *n_months* monthly periods at the current rate."""
    ...

def remaining_months() -> int:
    """Number of months remaining until policy maturity."""
    ...

def surrender_value() -> float:
    """Account value minus a graded surrender charge (linear from 10% to 0%)."""
    ...
