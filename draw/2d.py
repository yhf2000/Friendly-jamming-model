from matplotlib import pyplot as plt

from getData import read_data_2

plt.figure(figsize=(8, 5))

i_list = [0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5]


for i in i_list:
    X, Y = read_data_2("with", i, "_with_C=10")
    plt.plot(X, Y, label='p=' + str(i) + " with")

X, Y = read_data_2("without", 1, "_with_C=10")
plt.plot(X, Y, label='p=1 without', linestyle=':')


plt.ylim(0, 12000)
plt.legend(bbox_to_anchor=(1, 0.47))
plt.show()
