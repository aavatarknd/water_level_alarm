from flask import Flask, request
import requests

app = Flask(__name__)

PUSHBULLET_API_KEY = 'YOUR_PUSHBULLET_API_KEY'
PUSHBULLET_API_URL = 'https://api.pushbullet.com/v2/pushes'

@app.route('/alarm', methods=['POST'])
def alarm():
    sensor_value = request.form.get('sensorValue')
    if int(sensor_value) > 512:
        headers = {
            'Access-Token': PUSHBULLET_API_KEY,
            'Content-Type': 'application/json'
        }
        payload = {
            'type': 'note',
            'title': 'Water Level Alarm',
            'body': f'Water level sensor value: {sensor_value}'
        }
        requests.post(PUSHBULLET_API_URL, json=payload, headers=headers)
    return 'OK', 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80)