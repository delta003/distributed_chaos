from flask import Flask, jsonify, request
from controllers.bootstrap_controller import *
from api.api_responses import *

app = Flask('bootstrap')
app.debug = True


# defaults
@app.route('/', methods=['POST', 'GET'])
def default_response():
    ok_response({})


# bootstrap API
@app.route('/api/hello', methods=['POST'])
def hello():
    try:
        [ip, port] = extract_data(request, ['ip', 'port'])
    except InvalidJsonRequest:
        return error_response('Invalid JSON in request')
    except:
        return error_response('Unknown error')
    try:
        response = hello_controller(ip, port)
    except:
        pass
    return ok_response(response)


@app.route('/api/reset', methods=['GET'])
def reset():
    try:
        response = reset_controller()
    except:
        pass
    return ok_response(response)


@app.route('/api/reset_done', methods=['GET'])
def reset_done():
    try:
        response = reset_done_controller()
    except:
        pass
    return ok_response(response)


# testing routes
@app.route('/test_receive', methods=['POST', 'GET'])
def test_receive():
    print("DEbUG")
    return ok_response({'message': 'sve je laz'})
    pass
