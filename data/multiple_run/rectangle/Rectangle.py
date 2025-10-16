import random
import numpy as np
import os


directions = [(1,0),(-1,0),(0,1),(0,-1)]


def initialize(length, width):
    machine = (random.randint(0,int(length)), random.randint(0,int(width)))
    path = [machine]
    return (machine, 1, path)



def RandomWalk (machine, walk_count, path, length, width, unique_visit):
    tempo = []
    for dx, dy in directions:
        newx = machine[0] + dx
        newy = machine[1] + dy
        if (newx >=0 and newx <= length and newy >= 0 and newy <= width):
            tempo.append((newx,newy))

    machine = random.choice(tempo)
    if machine not in unique_visit:
        walk_count += 1

    path.append(machine)
    unique_visit.add(machine)
    return machine, walk_count, path, unique_visit

def mainFunc(l, w, totalpoint):
    #times_running = 1000

    #initializing shape boundary
    length = l
    width = w

    machine, walk_count, path_ = initialize(length, width)
    
    unique_visit = set(path_)

    base_dir = os.path.dirname(os.path.abspath(__file__))
    txt_path = os.path.join(base_dir, "output.txt")
    with open(txt_path, "w") as f:
        f.write(f"{length} {width} \n")
        f.write(f"{machine[0]} {machine[1]} {len(path_)} {len(unique_visit)}\n")
        while (True):
            machine, walk_count, path_, unique_visit = RandomWalk(machine, walk_count, path_, length, width, unique_visit)
            f.write(f"{machine[0]} {machine[1]} {len(path_)} {len(unique_visit)}\n")
            if (totalpoint // 2 == walk_count):
                break
        


if __name__ == "__main__":
    mainFunc(165.7789267,55.25964223,9296)