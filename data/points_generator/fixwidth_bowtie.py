import numpy as np

newWidth = []
length = []

a = np.sqrt(np.pi / 45)
fix_width = 40

for m in range(10, 300):
    width = a*m
    length = 5 * width
    hypo_side = 4 * width

    height = 2*hypo_side + fix_width
    long = 2*hypo_side + length

    total = 0
    for i in range(int(long) + 1):
        for j in range(int(height) +1):
            if (i>= 0 and i <= hypo_side):
                if (j >= i and j <= (height - i)):
                    total += 1
            elif (i > hypo_side and i <= (hypo_side + length)):
                if (j >= hypo_side and j <= (hypo_side + fix_width)):
                    total += 1
            elif (i > (hypo_side + length) and i <= long):
                if (j >= (long-i) and j <= (i + fix_width - length)):
                    total += 1
    print(length)
