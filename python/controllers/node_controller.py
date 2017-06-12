def __init__(link_data, address_data, data, job_data):
    global links, addresses, node_data, jobs
    links = link_data
    addresses = address_data
    node_data = data
    jobs = job_data


def basic_ok_controller():
    return {}


def basic_info_controller():
    return {'uuid': node_data.uuid, 'ip': addresses.ip, 'port': addresses.port}
