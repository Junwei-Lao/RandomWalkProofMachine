import Circle
import Rectangle
import Square
import Triangle
import pandas

#Circle
file_circle = pandas.read_csv("./data/multiple_run/Circle.csv")
mean_list = []
std_list = []
j = 0
for i in file_circle["cirRadius"]:
    m, s = Circle.mainFunc(i)
    mean_list.append(m)
    std_list.append(s)
    print(j)
    j += 1

file_circle["mean"] = mean_list
file_circle["Std"] = std_list
file_circle.to_csv('Circle_results.csv', index=False)


#3-1 Rectangle
file_rec = pandas.read_csv("./data/multiple_run/Rectangle.csv")
mean_list = []
std_list = []
k = 0
for i, j in zip(file_rec["Length"], file_rec["width"]):
    m, s = Rectangle.mainFunc(i, j)
    mean_list.append(m)
    std_list.append(s)
    print(k)
    k += 1

file_rec["mean"] = mean_list
file_rec["Std"] = std_list
file_rec.to_csv('Rectangle_results.csv', index=False)


#Square
file_square = pandas.read_csv("./data/multiple_run/Square.csv")
mean_list = []
std_list = []
j = 0
for i in file_square["Edge"]:
    m, s = Square.mainFunc(i)
    mean_list.append(m)
    std_list.append(s)
    print(j)
    j += 1

file_square["mean"] = mean_list
file_square["Std"] = std_list
file_square.to_csv('Square_results.csv', index=False)


# Triangle
file_triangle = pandas.read_csv("./data/multiple_run/Triangle.csv")
mean_list = []
std_list = []
j = 0
for i in file_triangle["Side"]:
    m, s = Triangle.mainFunc(i)
    mean_list.append(m)
    std_list.append(s)
    print(j)
    j += 1

file_triangle["mean"] = mean_list
file_triangle["Std"] = std_list
file_triangle.to_csv('Triangle_results.csv', index=False)
