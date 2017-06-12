def hello_controller(ip, port):
    ret = {'uuid': bootstrap_data.uuid, 'ip': bootstrap_data.last_ip, 'port': bootstrap_data.last_port}
    bootstrap_data.inc_uuid()
    bootstrap_data.set_last_ip(ip)
    bootstrap_data.set_last_port(port)
    return ret


def reset_controller():
    if bootstrap_data.can_reset:
        bootstrap_data.set_can_reset(False)
        bootstrap_data.set_last_ip(None)
        bootstrap_data.set_last_port(None)
        return {'can_reset': 'true'}
    else:
        return {'can_reset': 'false'}


def reset_done_controller():
    bootstrap_data.set_can_reset(True)
    # TODO: call network reconfiguration here
    return {}


def __init__(model):
    global bootstrap_data
    bootstrap_data = model
