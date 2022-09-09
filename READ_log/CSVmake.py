f = open("a.log", "r")
while True:
    line = f.readline()
    if line == "time":
        print("time")
    if line == "acceleration":
        print("acceleration")
    if line == "MPU Aceleration":
        print("MPU Aceleration")
    if line == "MPU Angular Velocity":
        print("MPU Angular Velocity")
    if line == "MPU Magnetic Field":
        print("MPU Magnetic Field")
    if line == "LPS Pressure":
        print("LPS Pressure")
    line = line.strip()
    line = line.replace(" ", ",")
    # print(line)
    print(1)