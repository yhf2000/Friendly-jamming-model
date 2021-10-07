from matplotlib import pyplot as plt

from getData import read_data_2

plt.figure(figsize=(8, 5))

i_list = [0.2]

for i in i_list:
    X, Y = read_data_2("Global withJammer", i)

    plt.plot(X, Y, label='gb_p=' + str(i) + " with")

for i in i_list:
    X, Y = read_data_2("Global withoutJammer", i)

    plt.plot(X, Y, label='gb_p=' + str(i) + " without", linestyle=':')


plt.ylim(8000, 20000)
plt.legend(bbox_to_anchor=(1, 0.5))
plt.show()
