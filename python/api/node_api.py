from flask import Flask, send_from_directory
from controllers.node_controller import *
from communication.response_creator import *
from controllers.node_controller import NetworkWaitException

import os.path as path
import sys

git_root = path.abspath(path.join(__file__, "../../.."))

app = Flask('node', static_folder=git_root + '/angularjs/app/static')


@app.route('/')
def default_response():
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


# Node API
@app.route('/api/basic/ok', methods=['GET'])
def basic_ok():
    try:
        ret = basic_ok_controller()
    except Exception as e:
        return error_response(str(e))
    return ok_response(ret)


@app.route('/api/basic/info', methods=['GET'])
def basic_info():
    try:
        ret = basic_info_controller()
    except NetworkWaitException:
        return wait_response()
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
    try:
        ret = network_visualize_controller()
    except Exception as e:
        return error_response(str(e))
    return ok_response(ret)


# Jobs API
@app.route('/api/jobs/add/<int:jobid>', methods=['POST'])
def jobs_add(jobid):
    try:
        [width, height, p, points] = extract_data(request, ['width', 'height', 'p', 'points'])
    except Exception as e:
        return error_response(str(e))

    try:
        # TODO: extract points from json into array of pairs
        ret = jobs_add_controller(jobid=jobid, width=int(width), height=int(height), p=float(p), points=points)
    except Exception as e:
        return error_response(str(e))
    return ret


@app.route('/api/jobs/new', methods=['POST'])
def jobs_new():
    try:
        [width, height, p, points] = extract_data(request, fields=['width', 'height', 'p', 'points'])
    except Exception as e:
        return error_response(str(e))

    try:
        # TODO: extract points from json into array of pairs
        ret = jobs_new_controller(width=int(width), height=int(height), p=float(p), points=points)
    except Exception as e:
        return error_response(str(e))
    return ret


@app.route('/api/jobs/all', methods=['GET'])
def jobs_all():
    try:
        ret = jobs_all_controller()
    except Exception as e:
        return error_response(e)
    return ret


@app.route('/api/jobs/backup', methods=['POST'])
def jobs_backup():
    try:
        [uuid, jobid, point] = extract_data(request, fields=['uuid', 'jobid', 'point'])
    except Exception as e:
        return error_response(str(e))

    try:
        ret = jobs_backup_controller(uuid=uuid, job_id=jobid, point=point)
    except Exception as e:
        return error_response(str(e))
    return ret


@app.route('/api/jobs/remove/<int:jobid>', methods=['GET'])
def jobs_remove(jobid):
    try:
        ret = jobs_remove_controller(job_id=jobid)
    except Exception as e:
        return error_response(str(e))
    return ret


@app.route('/api/jobs/kill/<int:jobid>', methods=['GET'])
def jobs_kill(jobid):
    try:
        ret = jobs_kill_controller(job_id=jobid)
    except Exception as e:
        return error_response(str(e))
    return ret


@app.route('/api/jobs/ids', methods=['GET'])
def jobs_ids():
    return ok_response({'jobids': []})
    try:
        ret = jobs_ids_controller()
    except Exception as e:
        return error_response(str(e))
    return ret


@app.route('/api/jobs/data/<int:jobid>', methods=['GET'])
def jobs_data(jobid):
    try:
        ret = jobs_data_controller(job_id=jobid)
    except Exception as e:
        return error_response(str(e))
    return ret


@app.route('/api/jobs/visualize/<int:jobid>', methods=['GET'])
def jobs_visualize(jobid):
    try:
        ret = jobs_visualize_controller(job_id=jobid)
    except Exception as e:
        return error_response(str(e))
    return ret
