"""
User-defined actuarial projection hook.

This script is loaded by the C++ application at startup.
The `on_month` function is called once per (policy, scenario, month).

It takes NO arguments — instead it calls functions from the `actuarial`
module, which is a library of C++ functions that read/write live
in-memory data structures and perform computations using those data
structures, all owned and managed by the C++ application.
"""
import actuarial


def on_month():
    """Called each projection month.  Access C++ state via actuarial.*()."""

    month = actuarial.get_month()
    pol   = actuarial.get_policy_id()
    scn   = actuarial.get_scenario_id()

    # ── Call C++ computation functions ───────────────────────────────
    # These run real C++ code that reads the live in-memory context.
    nar      = actuarial.net_amount_at_risk()
    coi      = actuarial.cost_of_insurance()
    sv       = actuarial.surrender_value()
    rem      = actuarial.remaining_months()

    # Store computed results back into C++ memory for downstream use
    actuarial.set_user_var("nar", nar)
    actuarial.set_user_var("coi", coi)
    actuarial.set_user_var("surrender_value", sv)

    # ── Use C++ pv_factor to compute a present-value reserve ────────
    if rem > 0:
        pv = actuarial.pv_factor(rem)
        adue = actuarial.annuity_due_factor(rem)
        db = actuarial.get_death_benefit()
        premium = actuarial.get_premium()

        # Net-level premium reserve: PV(future benefits) - PV(future premiums)
        nlp_reserve = db * (1.0 - pv) - premium * adue * (1.0 - pv)
        actuarial.set_user_var("nlp_reserve", nlp_reserve)

    # ── Override the reserve with a custom formula ──────────────────
    av = actuarial.get_account_value()
    actuarial.set_reserve(av * 0.97)

    # ── Print a summary every 6 months ──────────────────────────────
    if month % 6 == 0:
        print(
            f"  [Python] pol={pol} scn={scn} month={month:>3}  "
            f"AV={av:>12,.2f}  COI={coi:>8,.2f}  "
            f"NAR={nar:>12,.2f}  SV={sv:>12,.2f}  "
            f"rem={rem:>3}mo"
        )
