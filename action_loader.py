import json
from typing import Any


def load_action_json(file_path: str) -> dict[str, Any]:
    with open(file_path, "r", encoding="utf-8") as f:
        return json.load(f)


def expand_action(action_name: str, action_dict: dict[str, Any]) -> list[dict[str, Any]]:
    if action_name not in action_dict:
        raise KeyError(f"action {action_name} not found in action.json")

    action = action_dict[action_name]
    if "sequence" not in action:
        return [action]

    expanded_steps: list[dict[str, Any]] = []
    for item in action["sequence"]:
        expanded_steps.extend(expand_action(item, action_dict))
    return expanded_steps
