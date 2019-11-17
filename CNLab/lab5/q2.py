import matplotlib.pyplot as plt
import numpy as np
import math

n = 10
p = 0.5
T = 3
no_of_itr = 10000
y_data = []
yn_data = []
x_data = []

for lmd in np.linspace(0, 1, 100):
    succ = 0
    t_time = 0
    q_len = np.zeros((n, ), dtype=np.int32)
    for itr in range(no_of_itr):
        for i in range(n):
            if np.random.rand() < lmd / n:
                q_len[i] += 1
        if t_time > 0:
            t_time -= 1
        else:
            no_of_packets = 0
            transmitting_i = -1
            for i in range(n):
                if q_len[i] > 0 and np.random.rand() < p:
                    no_of_packets += 1
                    transmitting_i = i
            if no_of_packets == 1:
                q_len[transmitting_i] -= 1
                succ += 1
                t_time = T
    y_data.append(succ / no_of_itr)
    yn_data.append(lmd / p * math.exp(-lmd / p))
    x_data.append(lmd)

plt.plot(x_data, y_data)
plt.show()