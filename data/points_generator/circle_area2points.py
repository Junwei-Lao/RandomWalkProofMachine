import pandas as pd
import numpy as np

r = []
area = []
points = []

for m in range(10, 300):
    radius = m
    total = 0
    for i in range(int(2*radius) + 1):
        for j in range(int(2*radius) + 1):
            if ((i-radius)**2 + (j-radius)**2 <= radius**2):
                total += 1


    Area = np.pi * radius**2
    r.append(radius)
    area.append(Area)
    points.append(total)
    print(m)


df = pd.DataFrame({
    'cirRadius': r,
    'cirArea': area,
    'cirPoints': points
})

# Save to CSV
df.to_csv('circle_result.csv', index=False)
