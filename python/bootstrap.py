from flask import Flask, jsonify, request
from utils import error_response

can_reset = True
next_uuid = 0
last_ip = None
last_port = None

app = Flask(__name__)


@app.route('/api/hello')
def hello():
    try:
        ip = request.data["ip"]

    except:
        pass
    finally:
        return error_response("Invalid JSON in request")

    return jsonify(response)


@app.route('/api/reset')
def reset():
    raise NotImplemented()


@app.route('/api/reset_done')
def reset_done():
    raise NotImplemented()
