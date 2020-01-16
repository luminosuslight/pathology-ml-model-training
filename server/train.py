from fastai.vision import *
from fastai.callbacks import *

from pathlib import Path
import os
import shutil


def unpack_data_and_train(params, model_id, train_data, valid_data):

    model_path = Path("models")/model_id
    if os.path.exists(model_path):
        shutil.rmtree(model_path, ignore_errors=True)

    os.makedirs(model_path)
    input_folder = model_path/'input'
    os.makedirs(input_folder)
    target_folder = model_path/'target'
    os.makedirs(target_folder)

    # unpack train data:
    for i, jpg_data in enumerate(train_data['inputImages']):
        path = input_folder/f"train_{i}.jpg"
        with open(path, 'wb') as file:
            file.write(jpg_data)

    for i, jpg_data in enumerate(train_data['targetImages']):
        path = target_folder/f"train_{i}.jpg"
        with open(path, 'wb') as file:
            file.write(jpg_data)

    # unpack validation data:
    for i, jpg_data in enumerate(valid_data['inputImages']):
        path = input_folder/f"valid_{i}.jpg"
        with open(path, 'wb') as file:
            file.write(jpg_data)

    for i, jpg_data in enumerate(valid_data['targetImages']):
        path = target_folder/f"valid_{i}.jpg"
        with open(path, 'wb') as file:
            file.write(jpg_data)

    # TODO: store model name

    train_unet(model_path, epochs=params['epochs'])


def train_unet(path, epochs):
    item_list = ImageImageList.from_folder(path/'input')
    item_lists = item_list.split_by_valid_func(lambda x: x.name.startswith('valid_'))
    label_lists = item_lists.label_from_func(lambda x: path/'target'/x.name)
    databunch = label_lists.databunch(bs=1)
    databunch.c = 3

    global train_progress
    train_progress.dataset_size = len(databunch.train_dl)

    wd = 1e-3
    y_range = (-3., 3.)
    loss_gen = MSELossFlat()

    arch = models.resnet18
    learn = unet_learner(databunch, arch, pretrained=True, wd=wd, blur=True, norm_type=NormType.Weight,
                         self_attention=True, y_range=y_range, loss_func=loss_gen, callbacks=[train_progress])
    # learn = learn.to_fp16()  # to save memory?

    learning_rate = 1e-3
    print("Training...")
    learn.fit_one_cycle(epochs, learning_rate)
    learn.export()
    print("Finished training and exported the model.")


class ProgressUpdateCallback(Callback):
    run_after = Recorder

    def __init__(self):
        self.progress = 0.0
        self.dataset_size = 1

    def on_batch_end(self, n_epochs, epoch, num_batch, iteration, **kwargs):
        print(num_batch, self.dataset_size, n_epochs, epoch)
        self.progress = (epoch * self.dataset_size + num_batch) / (n_epochs * self.dataset_size)
        print(self.progress)

train_progress = ProgressUpdateCallback()

