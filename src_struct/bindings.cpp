#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include "script_contexts.h"
#include "hook_registry.h"

namespace py = pybind11;

// ── Hook decorator ──────────────────────────────────────────────────
// Each instance (e.g. actuarial.mortality) is a decorator factory.
// Client scripts use:  @actuarial.mortality(name="Table1")
//                      def calc(ctx): ...
// The engine reads back:  actuarial.mortality.funcs["Table1"]

struct HookDecorator {
    std::string hook_name;
    py::dict    funcs;           // name → callable

    explicit HookDecorator(std::string n)
        : hook_name(std::move(n)) {}
};

PYBIND11_EMBEDDED_MODULE(actuarial, m) {
    m.doc() = "Actuarial projection library — struct passing";

    // ── Context structs ─────────────────────────────────────────────
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
        .def_readwrite("index_returns",     &EiaCreditContext::index_returns)
        .def_readwrite("credited_rate",     &EiaCreditContext::credited_rate);

    // ── Hook decorators — driven by HOOK_TABLE ──────────────────────
    py::class_<HookDecorator>(m, "HookDecorator")
        .def(py::init<std::string>())
        .def_readonly("hook_name", &HookDecorator::hook_name)
        .def_readonly("funcs",     &HookDecorator::funcs)
        .def("__call__", [](py::object self_obj, const std::string& name) {
            return py::cpp_function([self_obj, name](py::object fn) -> py::object {
                auto& self = self_obj.cast<HookDecorator&>();
                py::str key(name);
                if (self.funcs.contains(key))
                    throw std::runtime_error(
                        "Hook '" + self.hook_name + "' already has '"
                        + name + "' registered");
                self.funcs[key] = fn;
                return fn;
            });
        }, py::arg("name"));

    for (const auto& meta : HOOK_TABLE) {
        m.attr(meta.name.data()) = HookDecorator(std::string(meta.name));
    }
}
