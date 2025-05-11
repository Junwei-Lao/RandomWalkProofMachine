import Triangle
import pandas
import numpy as np
from concurrent.futures import ThreadPoolExecutor


input_file = "Triangle.csv"
output_file = "Triangle_results.csv"

file_rec = pandas.read_csv(input_file)
j = 0
# Write headers first (if creating new)
with open(output_file, mode='w', newline='') as f:
    f.write("Side,triPoints,mean,Std,middle\n")

for s, points in zip(file_rec["Side"], file_rec["triPoints"]):
    with ThreadPoolExecutor() as executor:
        results = list(executor.map(
            lambda _: Triangle.mainFunc(s, points),
            range(1000)
        ))

    result_array = np.array(results)
    mean = np.mean(result_array)
    std = np.std(result_array)
    median = np.median(result_array)

    # Append result to CSV immediately
    with open(output_file, mode='a', newline='') as f:
        f.write(f"{s},{points},{mean},{std},{median}\n")

    print(j)
    j += 1