import random
import numpy as np
import os

directions = [(1,0),(-1,0),(0,1),(0,-1)]

def initialize(hypo_side, length, width):
    height = 2*hypo_side + width
    long = 2*hypo_side + length
    while (True):
        machine = (random.randint(0,int(long)+1), random.randint(0,int(height)+1))

        if (machine[0] <= hypo_side):
            if (machine[1] >= machine[0] and machine[1] <= height - machine[0]):
                break
        elif (machine[0] > hypo_side and machine[0] <= hypo_side + length):
            if (machine[1] >= hypo_side and machine[1] <= hypo_side + width):
                break
        elif (machine[0] > hypo_side + length and machine[0] <= long):
            if (machine[1] >= long-machine[0] and machine[1] <= machine[0] + height - long):
                break

    path = [machine]
    return (machine, 1, path)


def RandomWalk (machine, walk_count, path, unique_visit, hypo_side, length, width):
    tempo = []
    height = 2*hypo_side + width
    long = 2*hypo_side + length
    for dx, dy in directions:
        newx = machine[0] + dx
        newy = machine[1] + dy

        if (newx >= 0 and newx <= hypo_side):
            if (newy >= newx and newy <= (height - newx)):
                tempo.append((newx,newy))
        elif (newx > hypo_side and newx <= (hypo_side + length)):
            if (newy >= hypo_side and newy <= (hypo_side + width)):
                tempo.append((newx,newy))
        elif (newx > (hypo_side + length) and newx <= long):
            if (newy >= (long-newx) and newy <= (newx + height - long)):
                tempo.append((newx,newy))

    machine = random.choice(tempo)
    if machine not in unique_visit:
        walk_count += 1
        
    path.append(machine)
    unique_visit.add(machine)
    return machine, walk_count, path, unique_visit

def mainFunc(w, l, hypo,totalpoints):

    width = w
    length = l
    hypo_side = hypo

    totalpoint = totalpoints

    machine, walk_count, path_ = initialize(hypo_side, length, width)
    
    unique_visit = set(path_)

    base_dir = os.path.dirname(os.path.abspath(__file__))
    txt_path = os.path.join(base_dir, "output_bowtie.txt")
    with open(txt_path, "w") as f:
        f.write(f"{width} \n {length} \n {hypo_side} \n")
        f.write(f"{machine[0]} {machine[1]} {len(path_)} {len(unique_visit)}\n")
        while (True):
            machine, walk_count, path_, unique_visit = RandomWalk(machine, walk_count, path_, unique_visit, hypo_side, length, width)
            f.write(f"{machine[0]} {machine[1]} {len(path_)} {len(unique_visit)}\n")
            if (totalpoint // 2 == walk_count):
                break
        


if __name__ == "__main__":
    mainFunc(14.26797827,71.33989136, 57.07191309 ,9217) #		