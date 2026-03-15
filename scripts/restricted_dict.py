"""
A dict subclass that only allows writing to predefined keys.
Used as the 'outputs' argument to enforce that scripts only set
the expected output variables.
"""


class RestrictedDict(dict):
    def __init__(self, allowed_keys):
        super().__init__()
        object.__setattr__(self, "_allowed", set(allowed_keys))

    def __setitem__(self, key, value):
        if key not in self._allowed:
            allowed = ", ".join(sorted(self._allowed))
            raise KeyError(
                f"'{key}' is not a valid output variable. "
                f"Allowed outputs: {allowed}"
            )
        super().__setitem__(key, value)
