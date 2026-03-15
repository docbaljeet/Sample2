#pragma once
#include <string>
#include <vector>
#include <unordered_map>

// ═══════════════════════════════════════════════════════════════════════
// Generic key-value context.
// Replaces per-script structs (MortalityContext, LapseContext, etc.)
// with a single type that holds any combination of named variables.
// ═══════════════════════════════════════════════════════════════════════

struct ScriptContext {
    std::unordered_map<std::string, int>                 ints;
    std::unordered_map<std::string, double>              doubles;
    std::unordered_map<std::string, std::vector<double>> vectors;
};
