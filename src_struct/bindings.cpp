#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include "script_contexts.h"

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(actuarial, m) {
    m.doc() = "Actuarial projection library — struct passing";

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
}
