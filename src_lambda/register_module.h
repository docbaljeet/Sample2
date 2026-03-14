#pragma once

struct ProjectionContext;

// Registers the "actuarial" Python module with all functions capturing
// the given pointer by reference.  Must be called after the Python
// interpreter is started and before any user script is imported.
void register_actuarial_module(ProjectionContext*& active);
