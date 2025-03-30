import pandas as pd
import numpy as np
from scipy.spatial import ConvexHull, Delaunay

side = []
area = []
points = []
a = 2 * np.sqrt(np.pi / np.sqrt(3))
b = np.sqrt(np.pi * np.sqrt(3))

for m in range(10, 300):

    side_length = a*m
    top = b*m

    boundary = [(0,0), (side_length, 0), (side_length/2, top)]
    boundary_np = np.array(boundary)
    boundary_hull = ConvexHull(boundary_np)
    boundary_delaunay = Delaunay(boundary_np[boundary_hull.vertices])

    total = 0
    for i in range(int(side_length) + 1):
        for j in range(int(top) + 1):
            tester1 = np.array((i, j))
            checker1 = boundary_delaunay.find_simplex(tester1) >= 0
            if (checker1):
                total += 1
            else:
                break


    Area = side_length * top / 2
    side.append(a*m)
    area.append(Area)
    points.append(total)
    print(m)


df = pd.DataFrame({
    'Side Length': side,
    'triArea': area,
    'triPoints': points
})

# Save to CSV
df.to_csv('triangle_result.csv', index=False)
