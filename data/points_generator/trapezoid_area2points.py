import pandas as pd
import numpy as np

side = []
area = []
points = []
a = np.sqrt(np.pi / 20)

for m in range(10, 300):

    length = a*m
    hypo_side = 4*length
    long = 2*hypo_side + length

    total = 0

    for i in range(int(long) + 1):
        inbound = True
        for j in range(int(hypo_side) + 1):
            if (i <= hypo_side and inbound):
                if (j <= i):
                    total += 1
                else:
                    inbound = False
            elif (i > hypo_side and i <= hypo_side+length and inbound):
                if (j <= hypo_side):
                    total += 1
                else:
                    inbound = False
            elif (i > hypo_side+length and i <= long and inbound):
                if (j <= -1*i + long):
                    total += 1
                else:
                    inbound = False
            else:
                break


    Area = 20 * length**2
    side.append(a*m)
    area.append(Area)
    points.append(total)
    print(m)


df = pd.DataFrame({
    'length': side,
    'traArea': area,
    'traPoints': points
})

# Save to CSV
df.to_csv('trapezoid_points_result.csv', index=False)
