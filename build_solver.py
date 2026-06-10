import os
import subprocess
import platform
import hashlib
import uuid


def hash_solver_directory(solver_dir: str) -> str:
    """
    solver_dir 以下のファイル内容をハッシュ化する。
    これにより、ファイル内容が変わったら自動で再ビルドできる。
    """
    h = hashlib.sha256()

    for root, dirs, files in os.walk(solver_dir):
        dirs.sort()
        files.sort()

        for f in files:
            # build成果物や不要ファイルは除外
            if f.endswith((".dll", ".so", ".dylib", ".a", ".o")):
                continue
            if f == ".build_hash":
                continue

            path = os.path.join(root, f)

            relpath = os.path.relpath(path, solver_dir).replace("\\", "/")
            h.update(relpath.encode("utf-8"))

            with open(path, "rb") as fp:
                h.update(fp.read())

    return h.hexdigest()


def get_output_library_path(solver_dir: str) -> str:
    system = platform.system().lower()

    if system == "windows":
        return os.path.join(solver_dir, "solver.dll")
    elif system == "darwin":
        return os.path.join(solver_dir, "libsolver.dylib")
    else:
        return os.path.join(solver_dir, "libsolver.so")


def detect_compiler(out_lib, sources):
    """Detect available compiler and return command list"""
    system = platform.system().lower()

    # Try MSVC first on Windows
    if system == "windows":
        try:
            subprocess.check_call(["cl", "/?"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            return ["cl", "/LD", "/Fe:" + out_lib] + sources  # MSVC syntax
        except (subprocess.CalledProcessError, FileNotFoundError):
            pass

    # Try clang
    try:
        subprocess.check_call(["clang", "--version"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        if system == "windows":
            return ["clang", "-shared", "-o", out_lib] + sources
        elif system == "darwin":
            return ["clang", "-shared", "-fPIC", "-o", out_lib] + sources
        else:
            return ["clang", "-shared", "-fPIC", "-o", out_lib] + sources
    except (subprocess.CalledProcessError, FileNotFoundError):
        pass

    # Fallback to gcc
    if system == "windows":
        return ["gcc", "-shared", "-o", out_lib] + sources
    elif system == "darwin":
        return ["gcc", "-shared", "-fPIC", "-o", out_lib] + sources
    else:
        return ["gcc", "-shared", "-fPIC", "-o", out_lib] + sources


def build_solver(force=False, out_lib=None):
    root_dir = os.path.dirname(__file__)
    solver_dir = os.path.join(root_dir, "solver")

    sources = [
        os.path.join(solver_dir, f)
        for f in sorted(os.listdir(solver_dir))
        if f.endswith(".c")
    ]
    if not sources:
        raise FileNotFoundError(f"No solver source files found in: {solver_dir}")

    if out_lib is None:
        out_lib = get_output_library_path(solver_dir)

    os.makedirs(os.path.dirname(out_lib), exist_ok=True)

    hash_file = os.path.join(solver_dir, ".build_hash")

    current_hash = hash_solver_directory(solver_dir)

    old_hash = None
    if os.path.exists(hash_file):
        with open(hash_file, "r", encoding="utf-8") as f:
            old_hash = f.read().strip()

    need_build = force or (not os.path.exists(out_lib)) or (old_hash != current_hash)

    if not need_build:
        return out_lib

    print("[BUILD] solver changed, compiling...")

    cmd = detect_compiler(out_lib, sources)

    print("[BUILD CMD]", " ".join(cmd))
    subprocess.check_call(cmd)

    if not os.path.exists(out_lib):
        raise RuntimeError("Build succeeded but output file not found.")

    # ハッシュ保存
    with open(hash_file, "w", encoding="utf-8") as f:
        f.write(current_hash)

    return out_lib


if __name__ == "__main__":
    lib = build_solver(force=False)
    print("Built:", lib)