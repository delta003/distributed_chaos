from flask import Flask, request
from controllers.node_controller import *
from api.api_responses import *

app = Flask('node')
app.debug = True


# defaults
@app.route('/')
def default_response():
    pass


# Node API
@app.route('/api/basic/ok', methods=['GET'])
def basic_ok():
    return ok_response(basic_ok_controller())


@app.route('/api/basic/info', methods=['GET'])
def basic_info():
    return ok_response(basic_info_controller())


@app.route('/api/basic/check', methods=['POST'])
def basic_check():
    # TODO: what is the usecase for this one?
    raise NotImplemented()


# Network API
@app.route('/api/network/edges', methods=['GET'])
def network_edges():
    return ok_response(network_edges_controller())


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
    pass


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
