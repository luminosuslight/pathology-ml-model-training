from flask import Flask
from flask import request, abort
import cbor2

from hashlib import md5
import os
import os.path
import sys
import threading

from inference import NeuralNetwork
from train import unpack_data_and_train

print("Python version:", sys.version)

default_network = NeuralNetwork('models/f81c0bd5273b967c2a9b980faac7dbb5/input')


UPLOAD_FOLDER = 'uploads'
if not os.path.exists(UPLOAD_FOLDER):
    os.makedirs(UPLOAD_FOLDER)

app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER


def secure_filename(filename):
    return filename.replace("/", "").replace("\\", "")


@app.route('/version', methods=['GET'])
def version():
    return "1.0", 200


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


# @app.route('/model', methods=['GET'])
# def all_models(model_id):
#    # get list of models
#    return list_of_models


@app.route('/model', methods=['POST'])
def train():
    raw_data = request.get_data()
    params = cbor2.loads(raw_data)
    dir(params)

    if params['baseModel']:
        model_id = f"{params['baseModel']}-{params['trainDataHash']}"
    else:
        model_id = params['trainDataHash']

    train_data = cbor2.loads(get_upload(params['trainDataHash']))
    valid_data = cbor2.loads(get_upload(params['validDataHash']))

    thread = threading.Thread(target=unpack_data_and_train,
                              args=(params, model_id, train_data, valid_data))
    thread.start()

    return model_id, 200


# @app.route('/model/<model_id>', methods=['GET'])
# def model_metadata(model_id):
#    # get metadata
#    return model_metadata


@app.route('/model/<model_id>/prediction/<hash>', methods=['GET'])
def predict(model_id, hash):
    if model_id != "default":
        print("Model not found:", model_id)
        abort(404)

    path = os.path.join(app.config['UPLOAD_FOLDER'], hash)
    if not os.path.isfile(path):
        print("Image not found:", hash)
        abort(404)

    print(f"Doing inference with model '{model_id}' and file {hash}...")

    output_img_data, centers = default_network.get_output_and_centers(path)

    print(f"Inference complete, now storing result and sending it back...")

    output_hash = store_in_uploads(output_img_data)

    result = {'outputImageHash': output_hash,
              'cellCenters': centers}

    cbor = cbor2.dumps(result)
    return cbor, 200


@app.route('/inference_progress', methods=['GET'])
def inference_progress():
    return str(default_network.progress), 200

# @app.route('/model/train/<data_hash>', methods=['GET'])
# def train(data_hash):
#    data = None
#    train, val = data
#    epoch_count = -1
#    # create dir
#    # unpack data there
#    # create metadata file
#    # train in other thread
#    return model_id
