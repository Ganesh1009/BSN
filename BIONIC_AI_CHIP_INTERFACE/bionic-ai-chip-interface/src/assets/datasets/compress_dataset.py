import json
import codecs

index=1

result = {}
result['samples'] = []
result['physiological'] = []
result['kinematics'] = []

maxHrTimestamp = 0
maxKinTimestamp = 0

with open('src/sessions/heartrate-03.json') as heartrate01:
    data = json.load(heartrate01)
    tshr_offset = data['data'][0][0]
    for p in data['data']:
        heartrate = {"physiological": {"heartrate": {
            "ts": (p[0]-tshr_offset), "hr": p[1]}}}
        # result['physiological'].append(heartrate)
        heartrate['ts'] = (p[0]-tshr_offset)
        result['samples'].append(heartrate)
        maxHrTimestamp = heartrate['ts']

# with open('src/sessions/kinematics-maria-01.json') as kinematics01:
with codecs.open('src/sessions/kinematics-knee-movement.json', 'r', 'utf-8-sig') as kinematics01:
    data = json.load(kinematics01)
    tskin_offset = data['samples'][0]['kinematics']['Sternum']['ts']
    for p in data['samples']:
        kinematics = p['kinematics']
        kinematics.pop('dummy', None)
        for s in p['kinematics']:
            p['kinematics'][s]['ts'] -= tskin_offset
            p['kinematics'][s]['ts'] /= 1000
        # result['kinematics'].append(p)
        p['ts'] = p['kinematics']['Sternum']['ts']
        result['samples'].append(p)
        maxKinTimestamp = p['kinematics']['Sternum']['ts']


def extract_time(json):
    try:
        # kinematics object ?
        return float(json['ts'])
    except KeyError:
        print('error key not found')
        return 0


result['samples'].sort(key=extract_time, reverse=False)

print(maxKinTimestamp, maxHrTimestamp, '\n')

with open('dataset_03.json', 'w') as outfile:
    json.dump(result['samples'], outfile, separators=(',', ':'))
