from flask import Flask
from controllers.node_controller import *
from requests.response_creator import *

app = Flask('node')
app.debug = True


# defaults
@app.route('/')
def default_response():
    pass


# Node API
@app.route('/api/basic/ok', methods=['GET'])
def basic_ok():
    try:
        ret = basic_ok_controller()
    except Exception as e:
        return error_response(str(e))
    return ok_response(ret)


# TODO: convert all values from json into appropriate data types before calling controller methods
@app.route('/api/basic/info', methods=['GET'])
def basic_info():
    try:
        ret = basic_info_controller()
    except Exception as e:
        return error_response(str(e))
    return ok_response(ret)


@app.route('/api/basic/check', methods=['POST'])
def basic_check():
    try:
        [ip, port] = extract_data(request, ['ip', 'port'])
    except Exception as e:
        return error_response(str(e))

    try:
        ret = basic_check_controller(ip, port)
    except Exception as e:
        return error_response(str(e))
    return ret


# Network API
@app.route('/api/network/edges', methods=['GET'])
def network_edges():
    try:
        ret = network_edges_controller()
    except Exception as e:
        return error_response(str(e))
    return ok_response(ret)


@app.route('/api/network/get_edge', methods=['POST'])
def network_get_edge():
    try:
        [type] = extract_data(request, ['type'])
    except Exception as e:
        return error_response(str(e))
    return ok_response(network_get_edge_controller(type))


@app.route('/api/network/set_edge', methods=['POST'])
def network_set_edge():
    try:
        [edge] = extract_data(request, ['edge'])
    except Exception as e:
        return error_response(str(e))
    return ok_response(network_set_edge_controller(edge))


@app.route('/api/network/adopt', methods=['POST'])
def network_adopt():
    try:
        [edge, can_redirect] = extract_data(request, ['edge', 'can_redirect'])
    except Exception as e:
        return error_response(str(e))
    return ok_response(network_adopt_controller(edge, can_redirect))


@app.route('/api/network/reset', methods=['GET'])
def network_reset():
    try:
        ret = network_reset_controller()
    except Exception as e:
        return error_response(str(e))

    return ret


@app.route('/api/network/visualize', methods=['GET'])
def network_visualize():
    pass


# Jobs API
@app.route('/api/jobs/add/<int:jobid>', methods=['POST'])
def jobs_add(jobid):
    pass


@app.route('/api/jobs/new', methods=['POST'])
def jobs_new():
    pass


@app.route('/api/jobs/all', methods=['GET'])
def jobs_all():
    pass


@app.route('/api/jobs/backup', methods=['POST'])
def jobs_backup():
    pass


@app.route('/api/jobs/remove/<int:jobid>', methods=['GET'])
def jobs_remove(jobid):
    pass


@app.route('/api/jobs/kill/<int:jobid>', methods=['GET'])
def jobs_kill(jobid):
    pass


@app.route('/api/jobs/ids', methods=['GET'])
def jobs_ids():
    pass


@app.route('/api/jobs/data/<int:jobid>', methods=['GET'])
def jobs_data(jobid):
    pass


@app.route('/api/jobs/visualize/<int:jobid>', methods=['GET'])
def jobs_visualize():
    pass
