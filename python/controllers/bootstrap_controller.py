from model.bootstrap_model import last_ip, can_reset, last_port, uuid
# TODO: make change visible in model.bootstrap_model

def hello_controller(ip, port):
    global last_ip, last_port, uuid
    ret = {'uuid': uuid, 'ip': last_ip, 'port': last_port}
    uuid = uuid + 1
    last_ip = ip
    last_port = port
    return ret


def reset_controller():
    global last_ip, last_port, can_reset
    if can_reset:
        can_reset = False
        last_ip = None
        last_port = None
        return {'can_reset': 'true'}
    else:
        return {'can_reset': 'false'}


def reset_done_controller():
    global can_reset
    can_reset = True
    # TODO: reconfigure network
    return {}
