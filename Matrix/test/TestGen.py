import shutil
import os
import json
import numpy as np
import random

def saveTest(num: int, name: str, path: str, test: str, ans: str) -> None:
    testName = name + str(num).zfill(3) + '.dat'
    ansName = testName + '.ans'
    with open(os.path.join(path, testName), 'w') as testFile:
        testFile.write(test)
    with open(os.path.join(path, ansName), 'w') as ansFile:
        ansFile.write(ans)

def generateRandomTest(config: dict):
    size = np.random.randint(config['matrixSizeRange']['min'], config['matrixSizeRange']['max'])
    diagElsRange = range(config['diagElemsRange']['min'], config['diagElemsRange']['max'], size)
    if config['integer']:
        diagElements = np.random.randint(low=config['diagElemsRange']['min'], high=config['diagElemsRange']['max'], size=size)
    else:
        diagElements = np.random.uniform(low=config['diagElemsRange']['min'], high=config['diagElemsRange']['max'], size=size)
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
            if config['integer']:
                mtxStr += '{:<6}\t'.format(el)
            else:
                mtxStr += '{:<{}.{}f}\t'.format(el, 6 + config['precision'], config['precision'])
        mtxStr += '\n'

    if config['integer']:
        detStr = '{}'.format(det)
    else:
        detStr = '{:.{}f}'.format(det, config['precision'])
    return [mtxStr, detStr]

def generateTests(config: dict):
    for conf in config:
        dirPath = os.path.join(os.path.dirname(__file__), conf['outputPath'])
        #if os.path.exists(dirPath):
        #    shutil.rmtree(dirPath)
        os.makedirs(dirPath, exist_ok=True)
        nTests = conf['nTests']

        for i in range(1, nTests + 1):
            test = generateRandomTest(conf)
            saveTest(i, conf['name'], dirPath, test[0], test[1])

def main ():
    configPath = os.path.join(os.path.dirname(__file__), 'config.json')
    if (not os.path.isfile(configPath)):
        raise Exception('Wrong config file path.')

    with open(configPath) as configFile:
        configSettings = json.load(configFile)
    
    generateTests(configSettings)

if __name__ == "__main__":
    main()
