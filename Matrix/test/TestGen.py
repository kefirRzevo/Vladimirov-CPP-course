import shutil
import os
import json
import numpy as np
import random

def saveTest(num: int, path: str, test: str, ans: str) -> None:
    testName = path + str(num).zfill(3) + '.dat'
    ansName = testName + '.ans'
    with open (os.path.join(path, testName), 'w') as testFile:
        testFile.write(test)
    with open (os.path.join(path, ansName), 'w') as ansFile:
        ansFile.write(ans)

def generateRandomTest(config: dict):
    size = np.random.randint(config['matrixSizeRange']['min'], config['matrixSizeRange']['max'])
    diagElsRange = range(config['diagElemsRange']['min'], config['diagElemsRange']['max'])
    diagElements = random.sample(diagElsRange, size)
    #print(diagElements)
    mtx = np.diag(diagElements)
    det = np.prod(diagElements)

    nTransforms = np.random.randint(config['elementaryTransformsRange']['min'], config['elementaryTransformsRange']['max'])
    for i in range(nTransforms):
        choice = np.random.choice([0, 1])
        if (choice == 0):
            pair = np.random.choice(a=np.arange(size), size=2, replace=False)
            sign = np.random.choice([-1, 1])
            mtx[pair[0]] += sign * mtx[pair[1]]
        else:
            pair = np.random.choice(a=np.arange(size), size=2, replace=False)
            mtx[[pair[0], pair[1]]] = mtx[[pair[1], pair[0]]]
            det *= -1

    mtxStr = ''
    mtxStr += '{}\n'.format(size)
    for row in mtx:
        for el in row:
            mtxStr += '{:<5}\t'.format(el)
        mtxStr += '\n'

    detStr = str(det)

    return [mtxStr, detStr]

def generateTests(config: dict):
    dirPath = os.path.join(os.path.dirname(__file__), config['outputPath'])
    if os.path.exists(dirPath):
        shutil.rmtree(dirPath)
    os.makedirs(dirPath, exist_ok=True)
    nTests = config['nTests']

    for i in range(1, nTests + 1):
        test = generateRandomTest(config)
        saveTest (i, dirPath, test[0], test[1])

def main ():
    configPath = os.path.join(os.path.dirname(__file__), 'config.json')
    if (not os.path.isfile(configPath)):
        raise Exception('Wrong config file path.')

    with open(configPath) as configFile:
        configSettings = json.load(configFile)
    
    generateTests(configSettings)

if __name__ == "__main__":
    main()
