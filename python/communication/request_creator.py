import requests
from config.bootstrap_config import bootstrap_ip, bootstrap_port


def create_ip_request(method, ip, port, endpoint, data={}):
    if not endpoint.startswith("/"):
        endpoint = "/" + endpoint
    address = "http://%s:%s%s" % (ip, port, endpoint)
    if method == 'GET':
        return requests.get(address, json=data).json()
    else:
        return requests.post(address, json=data).json()


def create_net_request(method, edge, endpoint, data={}):
    return create_ip_request(method=method, ip=edge['ip'], port=edge['port'], endpoint=endpoint, data=data)


def create_bootstrap_request(method, endpoint, data={}):
    return create_ip_request(method, endpoint=endpoint, ip=bootstrap_ip, port=bootstrap_port, data=data)


# Bootstrap API
def bootstrap_hello(ip, port):
    ret = create_ip_request(method='POST', endpoint='/api/hello', ip=bootstrap_ip, port=bootstrap_port,
                            data={'ip': ip, 'port': port})

    if 'ip' not in ret:
        return ret['uuid'], None, None

    return ret['uuid'], ret['ip'], ret['port']


def bootstrap_reset():
    ret = create_ip_request(method='GET', endpoint='/api/reset', ip=bootstrap_ip, port=bootstrap_port)
    if ret['can_reset'] == 'true':
        return True
    return False


def bootstrap_reset_done():
    create_ip_request(method='GET', endpoint='/api/reset_done', ip=bootstrap_ip, port=bootstrap_port)


# Node basic API
def basic_ok(edge):
    create_net_request(method='GET', endpoint='/api/basic/ok', edge=edge)


def basic_info(edge):
    ret = create_net_request(method='GET', endpoint='/api/basic/info', edge=edge)
    return ret['uuid'], ret['ip'], ret['port']


def basic_check(edge, ip, port):
    ret = create_net_request(method='POST', endpoint='/api/basic/info', edge=edge,
                             data={'ip': ip, 'port': port})
    if ret['alive'] == 'true':
        return True
    return False


# Network API
def all_edges(edge):
    ret = create_net_request(method='GET', endpoint='/api/network/edges', edge=edge)
    return ret['edges']


def get_edge(edge, type):
    ret = create_net_request(method='POST', endpoint='/api/network/get_edge', edge=edge, data={'type': type})
    return ret['edge']


def set_edge(edge, e, type):
    e['type'] = type
    ret = create_net_request(method='POST', endpoint='/api/network/set_edge', edge=edge, data={'edge': e})
    return ret['oldedge']


def adopt_child(edge, e, can_redirect=False):  # TODO: not sure if third argument is fine
    request_data = {'edge': e}
    if can_redirect:
        request_data['can_redirect'] = 'true'
    else:
        request_data['can_redirect'] = 'false'
    ret = create_net_request(method='POST', edge=edge, endpoint='/api/network/adopt', data=request_data)
    redirect = False
    if 'redirect' in ret and ret['redirect'] == 'true':
        redirect = True

    create_level = True
    if 'create_level' in ret and ret['create_level'] == 'true':
        create_level = True

    edges = None
    if 'edges' in ret:
        edges = ret['edges']

    next = None
    if 'next' in ret:
        next = ret['next']

    return redirect, create_level, edges, next


def reset_node(edge):
    ret = create_net_request(method='GET', edge=edge, endpoint='/api/network/reset')
    return ret['edges']


def network_visualization(edge):
    ret = create_net_request(method='GET', edge=edge, endpoint='/api/network/visualize')
    return ret['nodes'], ret['edges']


# Jobs API
def add_job(edge, job_id, width, height, p, points):
    endpoint = '/api/jobs/add/%d' % job_id
    ret = create_net_request(method='POST', edge=edge, endpoint=endpoint,
                             data={'width': width, 'height': height, 'p': p, 'points': points})
    return ret['edges']


def new_job(edge, width, height, p, points):
    ret = create_net_request(method='POOST', edge=edge, endpoint='/api/jobs/new',
                             data={'width': width, 'height': height, 'p': p, 'points': points})
    return ret['jobid']


def all_jobs(edge):
    ret = create_net_request(method='GET', edge=edge, endpoint='/api/jobs/all')
    return ret['jobs']


def backup_jobs(edge, uuid, job_id, point):
    create_net_request(method='POST', edge=edge, endpoint='/api/jobs/backup',
                       data={'uuid': uuid, 'jobid': job_id, 'point': point})


def remove_job(edge, job_id):
    endpoint = '/api/jobs/remove/%d' % job_id
    ret = create_net_request(method='GET', edge=edge, endpoint=endpoint)
    return ret['edges']


def kill_job(edge, job_id):
    endpoint = '/api/jobs/kill/%d' %d job_id
    create_net_request(method='GET', edge=edge, endpoint=endpoint)


def list_ids(edge):
    ret = create_net_request(method='GET', edge=edge, endpoint='/api/jobs/ids')
    return ret['jobids']


def job_data(edge, job_id):
    endpoint = '/api/jobs/data/%d' % job_id
    ret = create_net_request(method='GET', edge=edge, endpoint=endpoint)
    return ret['uuid'], ret['points'], ret['backup'], ret['edges']


def job_visualization(edge, job_id):
    endpoint = '/api/jobs/visualize/%d' % job_id
    ret = create_net_request(method='GET', edge=edge, endpoint=endpoint)
    return ret['startingpoints'], ret['points'], ret['width'], ret['height']
