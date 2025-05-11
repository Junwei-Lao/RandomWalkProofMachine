import random
import numpy as np

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
    #times_running = 1
    radius = r

    #num_list = []
    totalpoint = totalpoints

    #for i in range(times_running):

    machine, walk_count, path = initialize(radius)
    unique_visit = set(path)

    while (True):
        machine, walk_count, path, unique_visit = RandomWalk(machine, walk_count, path, unique_visit, r)
        if (totalpoint // 2 == walk_count):
            break

    #num_list.append(len(path)-1)
    return len(path)-1
        
        

    

