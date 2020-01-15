from flask import Flask
from flask import request, abort
import cbor2

from hashlib import md5
import os
import os.path

from inference import NeuralNetwork

default_network = NeuralNetwork('/home/tim/Masterarbeit/artificial_data/input')


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


@app.route('/data/<hash>', methods=['GET'])
def download(hash):
    path = os.path.join(app.config['UPLOAD_FOLDER'], hash)
    if not os.path.isfile(path):
        abort(404)

    with open(path, 'rb') as file:
        content = file.read()

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
    cbor = cbor2.loads(raw_data)
    print(cbor.keys())
    print("Model Name:", cbor['modelName'])
    print("Base Model:", cbor['baseModel'])
    print("Epochs:", cbor['epochs'])
    print("Train Data Hash:", cbor['trainDataHash'])
    print("Validation Data Hash:", cbor['validDataHash'])
    if cbor['baseModel']:
        model_id = f"{cbor['baseModel']}-{cbor['trainDataHash']}"
    else:
        model_id = cbor['trainDataHash']

    model_path = "models/" + model_id
    if os.path.exists(model_path):
        os.removedirs(model_path)

    os.makedirs(model_path)
    input_folder = os.path.join(model_path, 'input')
    os.makedirs(input_folder)
    target_folder = os.path.join(model_path, 'target')
    os.makedirs(target_folder)

    # unpack train data:
    train_data_path = os.path.join(app.config['UPLOAD_FOLDER'], cbor['trainDataHash'])

    with open(train_data_path, 'rb') as file:
        train_data = cbor2.loads(file.read())

    for i, jpg_data in enumerate(train_data['inputImages']):
        path = os.path.join(input_folder, f"train_{i}.jpg")
        with open(path, 'wb') as file:
            file.write(jpg_data)

    for i, jpg_data in enumerate(train_data['targetImages']):
        path = os.path.join(target_folder, f"train_{i}.jpg")
        with open(path, 'wb') as file:
            file.write(jpg_data)

    # unpack validation data:
    valid_data_path = os.path.join(app.config['UPLOAD_FOLDER'], cbor['validDataHash'])

    with open(valid_data_path, 'rb') as file:
        valid_data = cbor2.loads(file.read())

    for i, jpg_data in enumerate(valid_data['inputImages']):
        path = os.path.join(input_folder, f"valid_{i}.jpg")
        with open(path, 'wb') as file:
            file.write(jpg_data)

    for i, jpg_data in enumerate(valid_data['targetImages']):
        path = os.path.join(target_folder, f"valid_{i}.jpg")
        with open(path, 'wb') as file:
            file.write(jpg_data)

    # TODO: store model name and train model

    

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
