import os
import re
from pathlib import Path
import subprocess
from flask import Flask, render_template, request, redirect, url_for, send_file

app = Flask(__name__)



DATA_DIR = Path(os.path.abspath(os.path.join(os.getcwd(), '..', 'data')))
DATA_DIR.mkdir(parents=True, exist_ok=True)


@app.route("/", methods=['GET', 'POST'])
def main():
    if request.method == 'GET':
        beta = request.args['beta'] if 'beta' in request.args else 6.0
        x0 = request.args['x0'] if 'x0' in request.args else 0.1
        if 'simulation_iterations' in request.args:
            simulation_iterations = request.args['simulation_iterations']
        else:
            simulation_iterations = 100_000
        started = request.args['started'] if 'started' in request.args else False
    elif request.method == 'POST':
        beta = request.form['beta']
        x0 = request.form['x0']
        simulation_iterations = request.form['simulation_iterations']
        subprocess.Popen(['cavity_scan', beta, x0, simulation_iterations, DATA_DIR])
        started = True
        return redirect(url_for('.main', beta=beta, x0=x0, simulation_iterations=simulation_iterations,
                                started=started))

    files = sorted([filename for filename in os.listdir(DATA_DIR) if not filename.startswith('.')])
    return render_template('main.html', beta=beta, x0=x0, simulation_iterations=simulation_iterations, started=started,
                           files=files)


@app.route("/load/<filename>", methods=['GET'])
def load(filename):
    if not re.match('^[a-zA-Z\.\d_]+$', filename):
        return redirect('/')
    return send_file(os.path.join(DATA_DIR, filename))


if __name__ == '__main__':
    app.run(debug=True)
