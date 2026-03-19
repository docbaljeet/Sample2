#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include "script_contexts.h"

namespace py = pybind11;

// ── Hook decorator ──────────────────────────────────────────────────
// Each instance (e.g. actuarial.mortality) is a callable decorator.
// Client scripts use:  @actuarial.mortality
//                      def calc(ctx): ...
// The engine reads back:  actuarial.mortality.func

struct HookDecorator {
    std::string name;
    py::object  func;

    explicit HookDecorator(std::string n)
        : name(std::move(n)), func(py::none()) {}

    py::object operator()(py::object fn) {
        if (!func.is_none())
            throw std::runtime_error(
                "Hook '" + name + "' is already registered");
        func = fn;
        return fn;                       // pass-through so the def still works
    }
};

PYBIND11_EMBEDDED_MODULE(actuarial, m) {
    m.doc() = "Actuarial projection library — struct passing";

    // ── Context structs (unchanged) ─────────────────────────────────
    py::class_<MortalityContext>(m, "MortalityContext")
        .def(py::init<>())
        .def_readonly("projection_months", &MortalityContext::projection_months)
        .def_readonly("issue_age",         &MortalityContext::issue_age)
        .def_readonly("gender",            &MortalityContext::gender)
        .def_readonly("smoker_status",     &MortalityContext::smoker_status)
        .def_readonly("face_amount",       &MortalityContext::face_amount)
        .def_readwrite("qx_monthly",       &MortalityContext::qx_monthly);

    py::class_<LapseContext>(m, "LapseContext")
        .def(py::init<>())
        .def_readonly("projection_months", &LapseContext::projection_months)
        .def_readonly("policy_term_years", &LapseContext::policy_term_years)
        .def_readonly("premium",           &LapseContext::premium)
        .def_readwrite("lapse_rate_monthly", &LapseContext::lapse_rate_monthly);

    py::class_<EiaCreditContext>(m, "EiaCreditContext")
        .def(py::init<>())
        .def_readonly("projection_months",  &EiaCreditContext::projection_months)
        .def_readonly("participation_rate", &EiaCreditContext::participation_rate)
        .def_readonly("cap_rate",           &EiaCreditContext::cap_rate)
        .def_readonly("floor_rate",         &EiaCreditContext::floor_rate)
        .def_readonly("spread",             &EiaCreditContext::spread)
        .def_property_readonly("index_returns",
            [](const EiaCreditContext& c) { return c.index_returns; })
        .def_readwrite("credited_rate",     &EiaCreditContext::credited_rate);

    // ── Hook decorators ─────────────────────────────────────────────
    py::class_<HookDecorator>(m, "HookDecorator")
        .def(py::init<std::string>())
        .def_readonly("name", &HookDecorator::name)
        .def_readwrite("func", &HookDecorator::func)
        .def("__call__", &HookDecorator::operator());

    // These are the only valid hook points — the engine's contract.
    m.attr("mortality")  = HookDecorator("mortality");
    m.attr("lapse")      = HookDecorator("lapse");
    m.attr("eia_credit") = HookDecorator("eia_credit");
}
