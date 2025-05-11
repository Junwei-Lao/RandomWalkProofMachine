import trapezoid
import pandas
import numpy as np
from concurrent.futures import ThreadPoolExecutor


input_file = "trapezoid.csv"
output_file = "trapezoid_results.csv"

file_rec = pandas.read_csv(input_file)
j = 0
# Write headers first (if creating new)
with open(output_file, mode='w', newline='') as f:
    f.write("length,traPoints,mean,Std,middle\n")

for l, points in zip(file_rec["length"], file_rec["traPoints"]):
    with ThreadPoolExecutor() as executor:
        results = list(executor.map(
            lambda _: trapezoid.mainFunc(l, points),
            range(1000)
        ))

    result_array = np.array(results)
    mean = np.mean(result_array)
    std = np.std(result_array)
    median = np.median(result_array)

    # Append result to CSV immediately
    with open(output_file, mode='a', newline='') as f:
        f.write(f"{l},{points},{mean},{std},{median}\n")

    print(j)
    j += 1