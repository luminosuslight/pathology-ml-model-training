from flask import Flask
from flask import request, abort, send_from_directory
from werkzeug import serving
from flask_cors import CORS
import cbor2

from hashlib import md5
import os
import os.path
import sys
import threading
import uuid

print("Python version:", sys.version)

UPLOAD_FOLDER = 'uploads'
if not os.path.exists(UPLOAD_FOLDER):
    os.makedirs(UPLOAD_FOLDER)

app = Flask(__name__)
CORS(app)
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER


def log_request(self, *args, **kwargs):
    # don't log progress requests:
    filters = ['training_progress', 'inference_progress']
    if any(filter in self.requestline for filter in filters):
        return
    _log_request(self, *args, **kwargs)


_log_request = serving.WSGIRequestHandler.log_request
serving.WSGIRequestHandler.log_request = log_request


def secure_filename(filename):
    return filename.replace("/", "").replace("\\", "")


@app.route('/version', methods=['GET'])
def version():
    return "1.0", 200


@app.route('/<path:path>', methods=['GET'])
def static_wasm_files(path):
    return send_from_directory('wasm', path), 200


@app.route('/projects/<path:path>', methods=['GET'])
def projects(path):
    return send_from_directory('projects', path), 200


def store_in_uploads(raw_data):
    hash = md5(raw_data).hexdigest()
    path = os.path.join(app.config['UPLOAD_FOLDER'], hash)
    with open(path, 'wb') as file:
        file.write(raw_data)
    return hash


@app.route('/data', methods=['POST'])
def upload():
    raw_data = request.get_data()
    hash = store_in_uploads(raw_data)
    return hash, 200


@app.route('/data/check/<hash>', methods=['GET'])
def check(hash):
    path = os.path.join(app.config['UPLOAD_FOLDER'], hash)
    return ("1" if os.path.isfile(path) else "0"), 200


def get_upload(file_hash):
    data_path = os.path.join(app.config['UPLOAD_FOLDER'], file_hash)
    if not os.path.isfile(data_path):
        abort(404)
    with open(data_path, 'rb') as file:
        content = file.read()
    return content


@app.route('/data/<hash>', methods=['GET'])
def download(hash):
    content = get_upload(hash)
    return content, 200


@app.route('/data/<hash>', methods=['DELETE'])
def delete_data(hash):
    path = os.path.join(app.config['UPLOAD_FOLDER'], hash)
    if not os.path.isfile(path):
        abort(404)

    os.remove(path)
    return '', 204


@app.route('/training_progress', methods=['GET'])
def training_progress():
    #return str(training_tracker.progress), 200
    return str(0.0), 200


@app.route('/inference_progress', methods=['GET'])
def inference_progress():
    #return str(max(NeuralNetwork.progress, TrainedAutoencoder.progress)), 200
    return str(0.0), 200
