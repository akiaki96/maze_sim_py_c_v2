import os
import re

def parse_action_enum(header_path:str):
    if not os.path.exists(header_path):
        raise FileNotFoundError(f"Header file not found: {header_path}")
    
    with open(header_path, 'r', encoding='utf-8', errors='replace') as file:
        text = file.read()
    
    # remove comments
    text = re.sub(r'//.*', '', text, flags=re.MULTILINE)  # remove single-line comments
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.DOTALL)  # remove multi-line comments

    m = re.search(r"typedef\s+enum\s*\{(.*?)\}\s*Action\s*;", text, flags=re.DOTALL)
    if not m:
        raise RuntimeError("Failed to find typedef enum {...} Action; in action.h")

    body = m.group(1)

    # 行ごとに処理
    entries = []
    for line in body.splitlines():
        line = line.strip()
        if not line:
            continue
        if line.endswith(","):
            line = line[:-1].strip()

        # 例: ACT_MOVE_1CELL = 1
        # 例: ACT_FINISH
        if "=" in line:
            name, val = [x.strip() for x in line.split("=", 1)]
            entries.append((name, val))
        else:
            entries.append((line, None))

    act2num = {}
    current_value = -1

    for name, val in entries:
        if val is None:
            current_value += 1
            act2num[name] = current_value
        else:
            # 数値は int(...) で評価できるようにする
            # 0x10 とかにも対応
            current_value = int(val, 0)
            act2num[name] = current_value

    num2act = {v: k for k, v in act2num.items()}

    return act2num, num2act

def load_action_enum():
    header_path = os.path.join("solver", "action.h")
    return parse_action_enum(header_path)

if __name__ == "__main__":
    # test parse_action_enum
    act2num, num2act = load_action_enum()
    print("act2num:", act2num)
    print("num2act:", num2act)
    # for name, val in act2num.items():
    #     print(f"{name} = {val}")