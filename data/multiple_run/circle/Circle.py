import random
import numpy as np
import os

'''
def totalPoints(radius, boundary_delaunay):
    total = 0
    for i in range(int(2*radius) + 1):
        for j in range(int(2*radius) +1):
            tester1 = np.array((i, j))
            checker1 = boundary_delaunay.find_simplex(tester1) >= 0
            if (checker1):
                total += 1
    return total
'''
directions = [(1,0),(-1,0),(0,1),(0,-1)]


path = []
unique_visit = set(path)
walk_count = 0



def initialize(radius):
    while (True):
        machine = (random.randint(0,int(2*radius)), random.randint(0,int(2*radius)))
        if ((machine[0] - radius)**2 + (machine[1] - radius)**2 <= radius**2):
            break # making sure the starting point of the machine is in the boundary
    path = [machine]
    return (machine, 1, path)


def RandomWalk (machine, walk_count, path, unique_visit, radius):
    tempo = []
    for dx, dy in directions:
        newx = machine[0] + dx
        newy = machine[1] + dy

        if ((newx - radius)**2 + (newy - radius)**2 <= radius**2):
            tempo.append((newx,newy))

    machine = random.choice(tempo)
    if machine not in unique_visit:
        walk_count += 1

    path.append(machine)
    unique_visit.add(machine)
    return machine, walk_count, path, unique_visit

def mainFunc(r, totalpoints):
    #global walk_count, path, unique_visit

    #times_running = 1
    radius = r

    #num_list = []
    totalpoint = totalpoints

    #for i in range(times_running):

    machine, walk_count, path_ = initialize(radius)
    unique_visit = set(path_)

    base_dir = os.path.dirname(os.path.abspath(__file__))
    txt_path = os.path.join(base_dir, "output.txt")
    with open(txt_path, "w") as f:
        f.write(f"{radius} \n")
        f.write(f"{machine[0]} {machine[1]} {len(path_)} {len(unique_visit)}\n")
        while (True):
            machine, walk_count, path_, unique_visit = RandomWalk(machine, walk_count, path_, unique_visit, r)
            f.write(f"{machine[0]} {machine[1]} {len(path_)} {len(unique_visit)}\n")
            if (totalpoint // 2 == walk_count):
                break




if __name__ == "__main__":
    mainFunc(54,9145)
        
        

    

