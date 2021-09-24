from matplotlib import pyplot as plt

from getData import read_data_2

plt.figure(figsize=(8, 5))

for i in range(10, 30, 4):
    X, Y = read_data_2("Time", i)

    plt.plot(X, Y, label='r=' + str(i))

plt.ylim(3.5, 5.8)
plt.legend(loc="lower right")
plt.show()
