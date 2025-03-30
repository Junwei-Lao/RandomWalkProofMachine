import random
import numpy as np
from scipy.spatial import ConvexHull, Delaunay

def totalPoints(side_length, top, boundary_delaunay):
    total = 0
    for i in range(int(side_length) + 1):
        for j in range(int(top) + 1):
            tester1 = np.array((i, j))
            checker1 = boundary_delaunay.find_simplex(tester1) >= 0
            if (checker1):
                total += 1
    return total


def initialize(side_length, top, boundary_delaunay):
    while (True):
        machine = (random.randint(0,int(side_length)), random.randint(0,int(top)))
        tester0 = np.array(machine)
        checker0 = boundary_delaunay.find_simplex(tester0) >= 0
        if (checker0):
            break # making sure the starting point of the machine is in the boundary
    path = [machine]
    return (machine, 1, path)


def RandomWalk (machine, walk_count, path, boundary_delaunay):
    directions = [(1,0),(-1,0),(0,1),(0,-1)]
    tempo = []
    for dx, dy in directions:
        newx = machine[0] + dx
        newy = machine[1] + dy
        tester = np.array([(newx, newy)])

        #check if the next step is in the boundary
        checker = boundary_delaunay.find_simplex(tester) >= 0

        if (checker[0]):
            tempo.append((newx,newy))

    machine = random.choice(tempo)
    if machine not in path:
        walk_count += 1

    path.append(machine)
    return machine, walk_count, path

def mainFunc(s):
    times_running = 1000

    #initializing shape boundary
    side_length = s
    top = np.sqrt(3) * (side_length/2)

    boundary = [(0,0), (side_length, 0), (side_length/2, top)]
    boundary_np = np.array(boundary)
    boundary_hull = ConvexHull(boundary_np)
    boundary_delaunay = Delaunay(boundary_np[boundary_hull.vertices])

    num_list = []
    totalpoint = totalPoints(side_length, top, boundary_delaunay)
    for i in range(times_running):
        machine, walk_count, path = initialize(side_length, top, boundary_delaunay)
        while (True):
            machine, walk_count, path = RandomWalk(machine, walk_count, path, boundary_delaunay)

            if (totalpoint // 2 == walk_count):
                break

        num_list.append(len(path)-1)
        

    #histogram initialize
    num_list_np = np.array(num_list)
    mean = np.mean(num_list_np)
    std = np.std(num_list_np)

    return mean, std

