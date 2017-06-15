from flask import Flask, send_from_directory

from controllers.bootstrap_controller import *
from communication.response_creator import *
import os.path as path
git_root = path.abspath(path.join(__file__, "../../.."))

app = Flask('bootstrap', static_folder=git_root+'/angularjs/app/static')
app.debug = True


# visualization
@app.route('/')
def default_response():
    print (git_root + '/angularjs/app/index.html')
    return send_from_directory(git_root + '/angularjs/app', filename="index.html")

@app.route('/controller/<path:filename>')
def serve_controller(filename):
    return send_from_directory(git_root + '/angularjs/app/controller', filename=filename)


@app.route('/service/<path:filename>')
def serve_service(filename):
    return send_from_directory(git_root + '/angularjs/app/service', filename=filename)


@app.route('/app.js')
def serve_appjs():
    return send_from_directory(git_root + '/angularjs/app', filename='app.js')


@app.route('/view/<path:filename>')
def serve_views(filename):
    return send_from_directory(git_root + '/angularjs/app/view', filename=filename)


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
