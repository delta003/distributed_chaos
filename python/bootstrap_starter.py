from api.bootstrap_api import app
from config.bootstrap_config import ip, port
from models.bootstrap_model import BootstrapData
import controllers.bootstrap_controller

if __name__ == '__main__':
    global bootstrap_data
    bootstrap_data = BootstrapData()
    controllers.bootstrap_controller.__init__(bootstrap_data)
    app.run(ip, port=port)
