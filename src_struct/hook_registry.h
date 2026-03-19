#pragma once
#include <pybind11/pytypes.h>
#include <array>
#include <cstddef>
#include <string_view>

namespace py = pybind11;

// ── 1. Add new hooks here — this is the ONLY place ─────────────
enum class Hook : std::size_t {
    Mortality,
    Lapse,
    EiaCredit,
    // ← new hooks go here
    Count
};

// ── 2. Metadata table — maps enum → Python name ────────────────
struct HookMeta {
    Hook             id;
    std::string_view name;   // Python-visible name
};

inline constexpr std::array<HookMeta, static_cast<std::size_t>(Hook::Count)> HOOK_TABLE {{
    { Hook::Mortality,  "mortality"  },
    { Hook::Lapse,      "lapse"      },
    { Hook::EiaCredit,  "eia_credit" },
}};

// ── 3. Runtime storage — flat array, O(1) indexed by enum ──────
class HookRegistry {
public:
    void set(Hook h, py::object fn) {
        slots_[static_cast<std::size_t>(h)] = std::move(fn);
    }

    const py::object& get(Hook h) const {
        return slots_[static_cast<std::size_t>(h)];
    }

    template <typename Ctx>
    void call(Hook h, Ctx& ctx) const {
        const auto& fn = slots_[static_cast<std::size_t>(h)];
        if (!fn.is_none()) {
            fn(py::cast(ctx, py::return_value_policy::reference));
        }
    }

private:
    std::array<py::object, static_cast<std::size_t>(Hook::Count)> slots_;
};
