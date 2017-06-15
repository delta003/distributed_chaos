import sys
from api.node_api import app
from config.bootstrap_config import bootstrap_ip as bootstrap_ip
from config.bootstrap_config import bootstrap_port as bootstrap_port
import controllers.node_controller
from models.node_model import NodeAddresses, NodeJobData, NodeData, NodeLinks
import threading
import os


if __name__ == '__main__':
    if len(sys.argv) > 3:
        print("Ignoring ip and port for bootstrap!!!")
    try:
        ip = '127.0.0.1'
        port = sys.argv[1]
        # bootstrap_ip = sys.argv[3]
        # bootstrap_port = sys.argv[4]
    except IndexError:
        print("Invalid number of parameters. Quitting...")
        quit()
    links = NodeLinks()
    addresses = NodeAddresses(ip=ip, port=port, bootstrap_ip=bootstrap_ip, bootstrap_port=bootstrap_port)
    node_data = NodeData()
    job_info = NodeJobData()
    controllers.node_controller.__init__(link_data=links, address_data=addresses,
                                         data=node_data, job_data=job_info, app_logger=app.logger)
    # When in debug mode, node_starter.py is executed two times. Be careful!

    app.logger.info('Starting node joining')
    join_thrd = threading.Thread(target=controllers.node_controller.join)
    join_thrd.start()
    #controllers.node_controller.ping()

    app.run(port=int(port), debug=True, use_reloader=False)

