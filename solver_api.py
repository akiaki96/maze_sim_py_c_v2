import ctypes
import json
import os
from build_solver import build_solver, get_output_library_path

class uint8_vector(ctypes.Structure):
    _fields_ = [
        ("value", ctypes.c_uint8 * 256),
        ("size", ctypes.c_int16)
    ]

class SolverAPI:
    def __init__(self, config_path: str = "solver_config.json"):
        # Load solver configuration
        if not os.path.exists(config_path):
            raise FileNotFoundError(f"Solver config file not found: {config_path}")
        
        with open(config_path, 'r', encoding='utf-8') as f:
            self.config = json.load(f)
        
        self.available_solvers = {s['name']: s for s in self.config['available_solvers']}
        self.default_solver = self.config['default_solver']
        
        dll_path = build_solver(force=False)
        self.current_library_path = dll_path
        self._load_library(dll_path)

    def _load_library(self, dll_path: str):
        self.dll = ctypes.CDLL(dll_path)
        self.current_library_path = dll_path

        self.dll.solver_init_pos.argtypes = []
        self.dll.solver_init_pos.restype = None

        self.dll.solver_init_map.argtypes = []
        self.dll.solver_init_map.restype = None

        # Dynamically setup solver functions based on config
        for solver in self.config['available_solvers']:
            solver_func_name = solver['solver_function']
            
            solver_func = getattr(self.dll, solver_func_name)
            solver_func.argtypes = [ctypes.c_bool, ctypes.c_bool, ctypes.c_bool]
            solver_func.restype = uint8_vector
            
            # Setup all init variants
            for variant in solver.get('init_variants', []):
                init_func_name = variant['init_function']
                init_func = getattr(self.dll, init_func_name)
                init_func.argtypes = []
                init_func.restype = uint8_vector

    def call_init_function(self, function_name: str):
        if not hasattr(self.dll, function_name):
            raise AttributeError(f"Function not found in solver library: {function_name}")

        func = getattr(self.dll, function_name)
        func.argtypes = []
        func.restype = uint8_vector
        return func()

    def init_pos(self):
        self.dll.solver_init_pos()

    def init_map(self):
        self.dll.solver_init_map()

    def get_solver_list(self):
        """Available solver names."""
        return list(self.available_solvers.keys())
    
    def get_solver_info(self, solver_name: str):
        """Get solver metadata."""
        if solver_name not in self.available_solvers:
            raise ValueError(f"Solver '{solver_name}' not found. Available: {self.get_solver_list()}")
        return self.available_solvers[solver_name]

    def get_init_variants(self, solver_name: str) -> list[dict]:
        solver_info = self.get_solver_info(solver_name)
        return solver_info.get("init_variants", [])

    def get_solver_python_init_function(self, solver_name: str, variant_name: str | None = None):
        """Get Python init function name for the specified solver and variant."""
        variants = self.get_init_variants(solver_name)
        
        if variant_name:
            variant = next((v for v in variants if v["variant_name"] == variant_name), None)
            if not variant:
                raise ValueError(f"Variant '{variant_name}' not found for solver '{solver_name}'")
        else:
            variant = variants[0] if variants else None
        
        return variant.get("python_init_function") if variant else None
    
    def solver_init(self, solver_name: str, variant_name: str | None = None):
        """Call initialization function for the specified solver and variant."""
        solver_info = self.get_solver_info(solver_name)
        
        if variant_name:
            variants = self.get_init_variants(solver_name)
            variant = next((v for v in variants if v["variant_name"] == variant_name), None)
            if not variant:
                raise ValueError(f"Variant '{variant_name}' not found for solver '{solver_name}'")
            init_func_name = variant["init_function"]
        else:
            variants = self.get_init_variants(solver_name)
            if variants:
                init_func_name = variants[0]["init_function"]
            else:
                raise ValueError(f"No init variants found for solver '{solver_name}'")
        
        init_func = getattr(self.dll, init_func_name)
        return init_func()
    
    def solver_step(self, solver_name: str, left_wall: bool, front_wall: bool, right_wall: bool):
        """Call solver step function for the specified solver."""
        solver_info = self.get_solver_info(solver_name)
        solver_func = getattr(self.dll, solver_info['solver_function'])
        return solver_func(left_wall, front_wall, right_wall)
    
    # Legacy methods for backward compatibility
    def init_all(self):
        return self.solver_init("left_wall")

    def solver_left_wall(self, left_wall: bool, front_wall: bool, right_wall: bool):
        return self.solver_step("left_wall", left_wall, front_wall, right_wall)

    def adachi_init(self):
        return self.solver_init("adachi")

    def solver_adachi(self, left_wall: bool, front_wall: bool, right_wall: bool):
        return self.solver_step("adachi", left_wall, front_wall, right_wall)


if __name__ == "__main__":
    api = SolverAPI()
    api.init_all()
    result = api.solver_left_wall(True, False, True)
    print("Result size:", result.size)
    print("Result values:", list(result.value)[:result.size])
    