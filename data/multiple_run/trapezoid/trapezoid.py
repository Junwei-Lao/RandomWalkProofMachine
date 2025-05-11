import random
import numpy as np

directions = [(1,0),(-1,0),(0,1),(0,-1)]

def initialize(hypo_side, length):
    long = 2*hypo_side+length
    while (True):
        machine = (random.randint(0,int(long)), random.randint(0,int(hypo_side)))
        if (machine[0] <= hypo_side):
            if (machine[1] <= machine[0]):
                break 
        elif (machine[0] > hypo_side and machine[0] <= hypo_side+length):
            if (machine[1] <= hypo_side):
                break
        elif (machine[0] > hypo_side+length and machine[0] <= long):
            if (machine[1] <= (-1)*machine[0] + long):
                break
    path = [machine]
    return (machine, 1, path)


def RandomWalk (machine, walk_count, path, hypo_side, length, unique_visit):
    long = 2*hypo_side+length
    tempo = []
    for dx, dy in directions:
        newx = machine[0] + dx
        newy = machine[1] + dy
        if (newx >= 0 and newx <= hypo_side):
            if (newy >= 0 and newy <= newx):
                tempo.append((newx,newy)) 
        elif (newx > hypo_side and newx <= hypo_side + length):
            if (newy >= 0 and newy <= hypo_side):
                tempo.append((newx,newy)) 
        elif (newx > hypo_side + length and newx <= long):
            if (newy >= 0 and newy <= (-1)*newx + long):
                tempo.append((newx,newy))

    machine = random.choice(tempo)
    if machine not in unique_visit:
        walk_count += 1

    path.append(machine)
    unique_visit.add(machine)
    return machine, walk_count, path, unique_visit

def mainFunc(l, totalpoint):
    length = l
    hypo_side = 4*l

    machine, walk_count, path = initialize(hypo_side, length)
    
    unique_visit = set(path)

    while (True):
        machine, walk_count, path, unique_visit = RandomWalk(machine, walk_count, path, hypo_side, length, unique_visit)

        if (totalpoint // 2 == walk_count):
            break
        

    return len(path)-1
