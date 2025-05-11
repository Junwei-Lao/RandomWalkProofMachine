import random
import numpy as np


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
        if (newx >= 0 and newx <= length and newy >= 0 and newy <= width):
            tempo.append((newx,newy))

    machine = random.choice(tempo)
    if machine not in unique_visit:
        walk_count += 1

    path.append(machine)
    unique_visit.add(machine)
    return machine, walk_count, path, unique_visit

def mainFunc(s, totalpoint):
    length = s
    width = s

    machine, walk_count, path = initialize(length, width)
    
    unique_visit = set(path)

    while (True):
        machine, walk_count, path, unique_visit = RandomWalk(machine, walk_count, path, length, width, unique_visit)

        if (totalpoint // 2 == walk_count):
            break
        

    return len(path)-1
