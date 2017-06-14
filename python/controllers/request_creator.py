import requests
from config.bootstrap_config import bootstrap_ip, bootstrap_port


def create_request(method, ip, port, endpoint, fields=[], data={}):
    if not endpoint.startswith("/"):
        endpoint = "/" + endpoint
    address = "http://%s:%s%s" % (ip, port, endpoint)
    ret = []
    if method == 'GET':
        json_data = requests.get(address, json=data).json()
        # TODO: add try/except here for request error and for invalid json
        for field in fields:
            ret.append(json_data[field])
    else:
        json_data = requests.post(address, json=data).json()
        # TODO: add try/except here for request error and for invalid json
        for field in fields:
            ret.append(json_data[field])
    return ret


def create_bootstrap_request(method, endpoint, fields=[], data={}):
    return create_request(method, endpoint=endpoint, ip=bootstrap_ip, port=bootstrap_port, data=data, fields=fields)
