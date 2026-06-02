import json
from collections import deque

from maze_logic import MousePos
from dir import Dir

class Action():
    def __init__(self, file_dir:str):
        self.action_json = open(file_dir, "r")
        self.action_dict = json.load(self.action_json)
        self.action_deque = deque()
        self.step_deque = deque()

    def push_action(self, action:str):
        if action not in self.action_dict:
            raise KeyError(f"action {action} not found in action_dict")
        self.action_deque.append(action)
    
    def push_step(self):
        if not self.action_deque:
            return
        now = self.action_deque.popleft()
        self.__push_step_rec(now)
    
    def __push_step_rec(self,now):
        if "sequence" not in self.action_dict[now]:
            self.step_deque.append(self.action_dict[now])
        else:
            for s in self.action_dict[now]["sequence"]:
                self.__push_step_rec(s)
    
    def pop_step(self, mousepos : MousePos):
        if not self.step_deque:
            return
        now = self.step_deque.popleft()
        if "MOVE" in now:
            mousepos.move(now["MOVE"])
        elif "ROTATE" in now:
            mousepos.rotate(now["ROTATE"])

    


def main():
    mousepos = MousePos(0,0,Dir.NORTH)
    action = Action("action.json")
    action.push_action("ACT_MOVE_1CELL")
    action.push_action("ACT_TURN_LEFT_MOVE")
    action.push_step()
    action.push_step()
    print(action.step_deque)
    i = len(action.step_deque)
    for _ in range(i):
        action.pop_step(mousepos)
        print(mousepos)

if __name__ == "__main__":
    main()