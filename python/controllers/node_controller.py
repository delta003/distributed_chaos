from flask import Blueprint
from controllers.request_creator import create_request, create_bootstrap_request
import threading

app_blueprint = Blueprint('app_blueprint', 'node')


def __init__(link_data, address_data, data, job_data):
    global links, addresses, node_info, jobs
    links = link_data
    addresses = address_data
    node_info = data
    jobs = job_data


# ping next node every 1 sec
def ping():
    threading.Timer(1, ping).start()

    if links.should_wait():
        return

    failures = 0
    next_edge = links.get_next()
    next_ip = next_edge['ip']
    next_port = next_edge['port']

    prev_edge = links.get_prev()
    prev_ip = prev_edge['ip']
    prev_port = prev_edge['port']

    try:
        create_request(method='GET', endpoint='/api/basic/ok', ip=next_ip, port=next_port)
    except Exception as e:
        failures = failures + 1
        if failures == 5:
            try:
                # call /api/basic/check for next on prev node
                failures = 0
            except Exception as e:
                reconfigure()


def reconfigure():
    pass


# joining node into network
def join():
    links.set_wait(True)


@app_blueprint.before_app_first_request
def register_node():
    # TODO: put this in join method and remove app_blueprint
    request_data = {'ip': addresses.get_ip(), 'port': addresses.get_port()}
    uuid, ip, port = create_bootstrap_request(method='POST', endpoint='/api/hello',
                                              fields=['uuid', 'ip', 'port'], data=request_data)

    node_info.set_uuid(uuid)
    addresses.set_manager_ip(ip)
    addresses.set_manager_port(port)


# TODO: basic methods should not block
def basic_ok_controller():
    return {}


def basic_info_controller():
    return {'uuid': node_data.get_uuid(), 'ip': addresses.get_ip(), 'port': addresses.get_port()}


def basic_check_controller():
    pass


def network_edges_controller():
    ret = []
    if links.get_parent() is not None:
        ret.append(links.get_parent())

    if links.get_prev() is not None:
        ret.append(links.get_prev())

    if links.get_next() is not None:
        ret.append(links.get_next())

    ret = ret + links.get_children()
    return {"edges": ret}


def network_get_edge_controller(type):
    return links.get_edge_as_dict(type)


def network_set_edge_controller(edge):
    ret = links.get_edge_as_dict(type)
    links.set_edge(edge)
    return ret


def network_adopt_controller(edge, can_redirect):
    if can_redirect == 'true':
        can_redirect = True
    else:
        can_redirect = False
    children_count = len(links.get_children())
    if children_count == 0 or children_count == 1 or children_count == 3:
        links.add_child(uuid=edge['uuid'], ip=edge['ip'], port=edge['port'])
        return {'redirect': 'false', 'create_level': 'false'}
    elif children_count == 2:
        if can_redirect:
            ret = links.get_edge_as_dict('next')
            ret['redirect'] = 'true'
            return ret
        else:
            links.add_child(uuid=edge['uuid'], ip=edge['ip'], port=edge['port'])
            return {'redirect': 'false', 'create_level': 'false'}
    elif children_count == 6:
        links.add_child(uuid=edge['uuid'], ip=edge['ip'], port=edge['port'])
        ret = network_edges_controller()
        ret['redirect'] = 'false'
        ret['create_level'] = 'true'
        for i in range(3):
            links.remove_child_at(2)
        return ret
    else:
        # TODO: what to do if children_count is 4 or 5
        raise NotImplemented()
