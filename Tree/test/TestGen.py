import shutil
import os
import json
import random

def numbersInRange (elements: list, kmin: int, kmax: int):
    elements.sort();
    count = 0
    for i in elements:
        if (i >= kmin):
            if (i <= kmax):
                count += 1
            else:
                break
    return count

def saveTest (num: int, path: str, test: str, ans: str) -> None:
    testName = path + str(num).zfill(3) + '.dat'
    ansName = testName + '.ans'
    with open (os.path.join(path, testName), 'w') as testFile:
        testFile.write(test)
    with open (os.path.join(path, ansName), 'w') as ansFile:
        ansFile.write(ans)

def generateRandomTest (config: dict):
    nTests = config['nTests']
    nKeys = random.randint(config['nKeysRange']['min'], config['nKeysRange']['max'])
    nQs = random.randint(config['nQsRange']['min'], config['nQsRange']['max'])

    keys = random.sample(range(config['kRange']['min'], config['kRange']['max']), nKeys)
    keys = list(set(keys))
    nKeys = len(keys)
    qsMin = random.sample(range(config['kRange']['min'], config['kRange']['max']), nQs)
    qsMax = random.sample(range(config['kRange']['min'], config['kRange']['max']), nQs)
    qs = zip(qsMin, qsMax)

    testStr = ''
    ansStr = ''

    for i in keys:
        testStr += 'k {} '.format(i)

    for i in qs:
        testStr += 'q {} '.format(i[0]) + '{} '.format(i[1])
        ansStr += '{} '.format(numbersInRange(keys, min(i[0], i[1]), max(i[0], i[1])))

    return [testStr, ansStr]

def generateTests (config: dict):
    dirPath = config['outputPath']
    if os.path.exists(dirPath):
        shutil.rmtree(dirPath)
    os.makedirs(config['outputPath'], exist_ok=True)
    nTests = config['nTests']

    for i in range (1, nTests + 1):
        test = generateRandomTest (config)
        saveTest (i, config['outputPath'], test[0], test[1])

def main ():
    configPath = os.path.join(os.path.dirname(__file__), 'config.json')
    if (not os.path.isfile(configPath)):
        raise Exception('Wrong config file path.')

    with open(configPath) as configFile:
        configSettings = json.load(configFile)
    
    generateTests (configSettings)

if __name__ == "__main__":
    main()
