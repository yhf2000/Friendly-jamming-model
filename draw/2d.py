from matplotlib import pyplot as plt

from getData import read_data_2

plt.figure(figsize=(8, 5))

i_list = [(0.2, 'b'), (0.4, 'g'), (0.6, 'r'), (0.8, 'c')]


for i in i_list:
    X, Y = read_data_2("Global withJammer", i[0])
    plt.plot(X, Y, label='p=' + str(i[0]) + " with", color=i[1])

for i in i_list:
    X, Y = read_data_2("Global withoutJammer", i[0])

    plt.plot(X, Y, label='p=1 without', linestyle=':', color=i[1])


plt.ylim(0, 30000)
plt.legend(bbox_to_anchor=(1, 0.5))
plt.show()
