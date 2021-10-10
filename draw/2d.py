from matplotlib import pyplot as plt

from getData import read_data_2

plt.figure(figsize=(8, 5))

i_list = [(0.2, 'b'), (0.3, 'g'), (0.4, 'r'), (0.5, 'c'), (0.6, 'y')]


for i in i_list:
    X, Y = read_data_2("with", i[0], "_with_C=4")
    plt.plot(X, Y, label='p=' + str(i[0]) + " with", color=i[1])

X, Y = read_data_2("without", 1, "_with_C=4")
plt.plot(X, Y, label='p=1 without', linestyle=':')


plt.ylim(600, 2000)
plt.legend(bbox_to_anchor=(1, 0.5))
plt.show()
