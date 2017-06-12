from flask import Flask, jsonify, request
from utils import error_response

can_reset = True
next_uuid = 0
last_ip = None
last_port = None

app = Flask(__name__)


# defaults
@app.route('/', methods=['POST', 'GET'])
def default_response():
    pass


# bootstrap API
@app.route('/api/hello', methods=['POST'])
def hello():
    pass


@app.route('/api/reset', methods=['GET'])
def reset():
    pass


@app.route('/api/reset_done', methods=['GET'])
def reset_done():
    pass


# testing routes
@app.route('/test_receive', methods=['POST', 'GET'])
def test_receive():
    print("DEbUG")
    return jsonify({'status': 'ok', 'message': 'sve je laz'})
    pass
