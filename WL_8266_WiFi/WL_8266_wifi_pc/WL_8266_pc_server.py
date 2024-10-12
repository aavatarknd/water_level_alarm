from flask import Flask, request

app = Flask(__name__)

@app.route('/update')
def update():
    value = request.args.get('value')
    print(f"Received water level: {value}")
    return 'OK', 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80)