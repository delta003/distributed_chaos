from flask import Flask, request

app = Flask(__name__)


# defaults
@app.route('/')
def default_response():
    pass


# Node API
@app.route('/api/basic/ok', methods=['GET'])
def basic_ok():
    pass


@app.route('/api/basic/info', methods=['GET'])
def basic_info():
    pass


@app.route('/api/basic/check', methods=['POST'])
def basic_check():
    pass


# Network API
@app.route('/api/network/edges', methods=['GET'])
def network_edges():
    pass


@app.route('/api/network/get_edge', methods=['POST'])
def network_get_edge():
    pass


@app.route('/api/network/set_edge', methods=['POST'])
def network_set_edge():
    pass


@app.route('/api/network/adopt', methods=['POST'])
def network_adopt():
    pass


@app.route('/api/network/reset', methods=['GET'])
def network_reset():
    pass


@app.route('/api/network/visualize', methods=['GET'])
def network_visualize():
    pass


# Jobs API
@app.route('/api/jobs/add/<int:jobid>', methods=['POST'])
def jobs_add(jobid):
    pass


@app.route('/api/jobs/new', methods=['POST'])
def jobs_new():
    pass


@app.route('/api/jobs/all', methods=['GET'])
def jobs_all():
    pass


@app.route('/api/jobs/backup', methods=['POST'])
def jobs_backup():
    pass


@app.route('/api/jobs/remove/<int:jobid>', methods=['GET'])
def jobs_remove(jobid):
    pass


@app.route('/api/jobs/kill/<int:jobid>', methods=['GET'])
def jobs_kill(jobid):
    pass


@app.route('/api/jobs/data/<int:jobid>', methods=['GET'])
def jobs_new(jobid):
    pass


@app.route('/api/jobs/ids', methods=['GET'])
def jobs_ids():
    pass


@app.route('/api/jobs/data/<int:jobid>', methods=['GET'])
def jobs_data(jobid):
    pass


@app.route('/api/jobs/visualize/<int:jobid>', methods=['GET'])
def jobs_visualize():
    pass

