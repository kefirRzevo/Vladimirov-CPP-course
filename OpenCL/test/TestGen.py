import shutil
import os
import json
import random
import math

def saveTest (num: int, path: str, test: str, ans: str) -> None:
    testName = path + str(num).zfill(3) + '.dat'
    ansName = testName + '.ans'
    with open (os.path.join(path, testName), 'w') as testFile:
        testFile.write(test)
    with open (os.path.join(path, ansName), 'w') as ansFile:
        ansFile.write(ans)

def generateRandomTest (config: dict):
    size = random.randint(config['sizeRange']['min'], config['sizeRange']['max'])
    data = random.sample(range(config['dataRange']['min'], config['dataRange']['max']), size)

    testStr = '{} '.format(size)
    ansStr = ''

    for i in data:
        testStr += '{} '.format(i)

    data.sort()
    for i in data:
        ansStr += '{} '.format(i)

    return [testStr, ansStr]

def generateTests (config: dict):
    dirPath = os.path.join(os.path.dirname(__file__), config['outputPath'])
    if os.path.exists(dirPath):
        shutil.rmtree(dirPath)
    os.makedirs(dirPath, exist_ok=True)
    nTests = config['nTests']

    for i in range (1, nTests + 1):
        test = generateRandomTest (config)
        saveTest (i, dirPath, test[0], test[1])

def main ():
    configPath = os.path.join(os.path.dirname(__file__), 'config.json')
    if (not os.path.isfile(configPath)):
        raise Exception('Wrong config file path.')

    with open(configPath) as configFile:
        configSettings = json.load(configFile)
    
    generateTests (configSettings)

if __name__ == "__main__":
    main()
