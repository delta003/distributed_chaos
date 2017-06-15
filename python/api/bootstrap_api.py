from flask import Flask, send_from_directory

from controllers.bootstrap_controller import *
from communication.response_creator import *

app = Flask('bootstrap')
app.debug = True

# visualization
@app.route('/', methods=['GET', 'POST'])
def default_response():
    return ok_response({})


# bootstrap API
@app.route('/api/hello', methods=['POST'])
def hello():
    try:
        [ip, port] = extract_data(request, ['ip', 'port'])
    except InvalidJsonRequest:
        return error_response('Invalid JSON in request')
    except Exception as e:
        return error_response(str(e))
    try:
        response = hello_controller(ip, port)
    except Exception as e:
        return error_response(str(e))
    return ok_response(response)


@app.route('/api/reset', methods=['GET'])
def reset():
    try:
        response = reset_controller()
    except Exception as e:
        return error_response(str(e))
    return ok_response(response)


@app.route('/api/reset_done', methods=['GET'])
def reset_done():
    try:
        response = reset_done_controller()
    except Exception as e:
        return error_response(str(e))
    return ok_response(response)
