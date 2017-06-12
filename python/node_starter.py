import sys

from api.node_api import app
from config.bootstrap_config import ip as bootstrap_ip
from config.bootstrap_config import port as bootstrap_port
import controllers.node_controller
from models.node_model import NodeAdresses, NodeJobData, NodeData, NodeLinks

if __name__ == '__main__':
    if len(sys.argv) > 3:
        print("Ignoring ip and port for bootstrap!!!")
    try:
        ip = sys.argv[1]
        port = int(sys.argv[2])
        # bootstrap_ip = sys.argv[3]
        # bootstrap_port = sys.argv[4]
    except IndexError:
        print("Invalid number of parameters. Quitting...")
        quit()
    links = NodeLinks()
    addresses = NodeAdresses(ip=ip, port=port, bootstrap_ip=bootstrap_ip, bootstrap_port=bootstrap_port)
    node_data = NodeData()
    jobs = NodeJobData()
    controllers.node_controller.__init__(link_data=links, address_data=addresses, data=node_data, job_data=jobs)
    app.run(ip, port=port)
    # TODO: logic for joining node
    node_data.set_uuid(123)
    print("ASD")
