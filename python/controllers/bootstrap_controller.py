def hello_controller(ip, port):
    ret = {'uuid': bootstrap_data.get_uuid()}
    if bootstrap_data.get_last_ip() is not None:
        ret['ip'] = bootstrap_data.get_last_ip()
        ret['port'] = bootstrap_data.get_last_port()
    bootstrap_data.inc_uuid()
    bootstrap_data.set_last_ip(ip)
    bootstrap_data.set_last_port(port)
    return ret


def reset_controller():
    if bootstrap_data.can_reset():
        bootstrap_data.set_can_reset(False)
        bootstrap_data.set_last_ip(None)
        bootstrap_data.set_last_port(None)
        return {'can_reset': 'true'}
    else:
        return {'can_reset': 'false'}


def reset_done_controller():
    bootstrap_data.set_can_reset(True)
    return {}


def __init__(model):
    global bootstrap_data
    bootstrap_data = model
