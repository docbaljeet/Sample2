#pragma once
#include "projection_context.h"

// Actuarial computation functions that operate on a live ProjectionContext.
// These are pure C++ logic — exposed to Python via pybind11 so that user
// scripts can call them as library functions during the projection loop.
namespace actuarial_funcs {

// Present value of $1 at the current interest rate for n months.
double pv_factor(const ProjectionContext& ctx, int n_months);

// Net amount at risk: death benefit minus reserve.
double net_amount_at_risk(const ProjectionContext& ctx);

// Cost of insurance for the current month:
//   NAR * monthly mortality rate
double cost_of_insurance(const ProjectionContext& ctx);

// Annuity-due factor (adue) for n periods at the current monthly rate.
double annuity_due_factor(const ProjectionContext& ctx, int n_months);

// Remaining projection months based on policy term.
int remaining_months(const ProjectionContext& ctx);

// Surrender value: account value minus a surrender charge that
// grades down linearly over the policy term.
double surrender_value(const ProjectionContext& ctx);

} // namespace actuarial_funcs
