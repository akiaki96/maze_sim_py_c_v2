import ctypes
from build_solver import build_solver, get_output_library_path

class uint8_vector(ctypes.Structure):
    _fields_ = [
        ("value", ctypes.c_uint8 * 256),
        ("size", ctypes.c_int16)
    ]

class SolverAPI:
    def __init__(self):
        dll_path = build_solver(force=False)
        self.dll = ctypes.CDLL(dll_path)

        self.dll.solver_init_pos.argtypes = []
        self.dll.solver_init_pos.restype = None

        self.dll.solver_init_map.argtypes = []
        self.dll.solver_init_map.restype = None

        self.dll.solver_init_all.argtypes = []
        self.dll.solver_init_all.restype = uint8_vector

        self.dll.solver_left_wall.argtypes = [ctypes.c_bool, ctypes.c_bool, ctypes.c_bool]
        self.dll.solver_left_wall.restype = uint8_vector

    def init_pos(self):
        self.dll.solver_init_pos()

    def init_map(self):
        self.dll.solver_init_map()

    def init_all(self):
        return self.dll.solver_init_all()

    def solver_left_wall(self, left_wall: bool, front_wall: bool, right_wall: bool):
        return self.dll.solver_left_wall(left_wall, front_wall, right_wall)


if __name__ == "__main__":
    api = SolverAPI()
    api.init_all()
    result = api.solver_left_wall(True, False, True)
    print("Result size:", result.size)
    print("Result values:", list(result.value)[:result.size])
    