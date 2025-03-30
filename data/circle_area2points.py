import pandas as pd
import numpy as np
from scipy.spatial import ConvexHull, Delaunay

r = []
area = []
points = []

points_num = 812
angle = 0
angleC = 2*np.pi / points_num

for m in range(10, 300):
    radius = m
    boundary = []
    for i in range(points_num):
        boundary.append((radius + radius*np.cos(angle), radius + radius*np.sin(angle)))
        angle += angleC

    boundary_np = np.array(boundary)
    boundary_hull = ConvexHull(boundary_np)
    boundary_delaunay = Delaunay(boundary_np[boundary_hull.vertices])

    total = 0
    for i in range(int(2*radius) + 1):
        for j in range(int(2*radius) + 1):
            tester1 = np.array((i, j))
            checker1 = boundary_delaunay.find_simplex(tester1) >= 0
            if (checker1):
                total += 1


    Area = np.pi * radius**2
    r.append(radius)
    area.append(Area)
    points.append(total)


df = pd.DataFrame({
    'cirRadius': r,
    'cirArea': area,
    'cirPoints': points
})

# Save to CSV
df.to_csv('circle_result.csv', index=False)
