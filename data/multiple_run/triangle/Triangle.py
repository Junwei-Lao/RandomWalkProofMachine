import random
import numpy as np

directions = [(1,0),(-1,0),(0,1),(0,-1)]

def initialize(side, top):
    y_intersection= 2*top
    while (True):
        machine = (random.randint(0,int(side)+1), random.randint(0,int(top)+1))
        if (machine[0] <= side/2):
            if (machine[1] <= np.sqrt(3)*machine[0]):
                break 
        elif (machine[0] <= side):
            if (machine[1] <= (-1)*np.sqrt(3)*machine[0] + y_intersection):
                break
    path = [machine]
    return (machine, 1, path)


def RandomWalk (machine, walk_count, path, side, top, unique_visit):
    y_intersection= 2*top
    tempo = []
    for dx, dy in directions:
        newx = machine[0] + dx
        newy = machine[1] + dy
        if (newx >= 0 and newx <= side/2):
            if (newy >= 0 and newy <= np.sqrt(3)*newx):
                tempo.append((newx,newy)) 
        elif (newx > side/2 and newx <= side):
            if (newy >= 0 and newy <= (-1)*np.sqrt(3)*newx + y_intersection):
                tempo.append((newx,newy))

    machine = random.choice(tempo)
    if machine not in unique_visit:
        walk_count += 1

    path.append(machine)
    unique_visit.add(machine)
    return machine, walk_count, path, unique_visit

def mainFunc(s, totalpoint):
    side_length = s
    top = np.sqrt(3) * (side_length/2)

    machine, walk_count, path = initialize(side_length, top)
    
    unique_visit = set(path)

    while (True):
        machine, walk_count, path, unique_visit = RandomWalk(machine, walk_count, path, side_length, top, unique_visit)

        if (totalpoint // 2 == walk_count):
            break
        

    return len(path)-1
