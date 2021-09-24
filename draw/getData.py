import json

import numpy as np


def read_data(name):
    data: dict = json.loads(open("local_statistics.json").read())

    X = []
    Y = []
    Z = []

    R = {}

    for x in data:
        for y in data[x]:
            if y['name'] == name:
                for z in y['data']:
                    if z['r'] not in R:
                        R[z['r']] = 1
                    else:
                        R[z['r']] += 1

    for x in data:
        XX = []
        YY = []
        ZZ = []
        for y in data[x]:
            if y['name'] == name:
                for z in y['data']:
                    if R[z['r']] == len(data):
                        XX.append(float(x))
                        YY.append(float(z['r']))
                        ZZ.append(float(z['val']))
        X.append(XX)
        Y.append(YY)
        Z.append(ZZ)

    return np.array(X), np.array(Y), np.array(Z)


def read_data_2(name, r=10):
    data: dict = json.loads(open("Time.json").read())

    X = []
    Y = []

    R = {}
    for x in data:
        for y in data[x]:
            if y['name'] == name:
                for z in y['data']:
                    if z['r'] not in R:
                        R[z['r']] = 1
                    else:
                        R[z['r']] += 1

    for x in data:
        for y in data[x]:
            if y['name'] == name:
                for z in y['data']:
                    if R[z['r']] == len(data) and z['r'] == r:
                        X.append(float(x))
                        Y.append(float(z['val']))

    return np.array(X), np.array(Y)
