import matplotlib.pyplot as plt
import numpy as np
import math

n = 10
no_of_itr = 10000
y_data = []
yn_data = []
x_data = []

for lmd in np.linspace(0, 4, 400):
    succ = 0
    for itr in range(no_of_itr):
        no_of_packets = 0
        for i in range(n):
            if np.random.rand() < lmd / n:
                no_of_packets += 1
        if no_of_packets == 1:
            succ += 1
    y_data.append(succ / no_of_itr)
    yn_data.append(lmd * math.exp(-lmd))
    x_data.append(lmd)

plt.plot(x_data, y_data)
plt.plot(x_data, yn_data)
plt.show()