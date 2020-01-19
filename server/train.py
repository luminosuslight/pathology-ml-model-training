from fastai.vision import *
from fastai.callbacks import *

from pathlib import Path
import os
import shutil


def unpack_data_and_train(params, model_id, base_model, train_data, valid_data):

    model_path = Path("models")/model_id
    if os.path.exists(model_path):
        shutil.rmtree(model_path, ignore_errors=True)

    os.makedirs(model_path)
    input_folder = model_path/'input'
    os.makedirs(input_folder)
    target_folder = model_path/'target'
    os.makedirs(target_folder)

    if base_model:
        base_model_weights = Path("models")/base_model/'trained_model.pth'
    else:
        base_model_weights = ""

    # unpack train data:
    for i, jpg_data in enumerate(train_data['inputImages']):
        path = input_folder/f"train_{i}.png"
        with open(path, 'wb') as file:
            file.write(jpg_data)

    for i, jpg_data in enumerate(train_data['targetImages']):
        path = target_folder/f"train_{i}.png"
        with open(path, 'wb') as file:
            file.write(jpg_data)

    # unpack validation data:
    for i, jpg_data in enumerate(valid_data['inputImages']):
        path = input_folder/f"valid_{i}.png"
        with open(path, 'wb') as file:
            file.write(jpg_data)

    for i, jpg_data in enumerate(valid_data['targetImages']):
        path = target_folder/f"valid_{i}.png"
        with open(path, 'wb') as file:
            file.write(jpg_data)

    # TODO: store model name

    train_unet(model_path, base_model_weights, epochs=params['epochs'])


def train_unet(path, base_model_weights, epochs):
    item_list = ImageImageList.from_folder(path/'input')
    item_lists = item_list.split_by_valid_func(lambda x: x.name.startswith('valid_'))
    label_lists = item_lists.label_from_func(lambda x: path/'target'/x.name)
    databunch = label_lists.databunch(bs=1)
    databunch.c = 3

    global training_tracker
    training_tracker.dataset_size = len(databunch.train_dl)

    wd = 1e-3
    y_range = (-3., 3.)
    loss_gen = MSELossFlat()

    arch = models.resnet18
    learn = unet_learner(databunch, arch, pretrained=True, wd=wd, blur=True, norm_type=NormType.Weight,
                         self_attention=True, y_range=y_range, loss_func=loss_gen, callbacks=[training_tracker])
    # learn = learn.to_fp16()  # to save memory?

    if base_model_weights:
        print("Loading weights of base model...", base_model_weights)
        with open(base_model_weights, 'rb') as file:
            learn.load(file)

    print(learn.summary())

    learning_rate = 1e-3
    print("Training...")
    # at this point, the early layers are frozen and only the last layers are trained
    learn.fit_one_cycle(epochs, learning_rate)
    print("Finetuning...")
    # here we will also train the first layers with a small learning rate to finetune the result
    learn.unfreeze()
    learn.fit_one_cycle(1, max_lr=slice(1e-6, 1e-4))
    print("Saving...")
    with open(path/'trained_model.pth', 'wb') as file:
        learn.save(file)  # save for later finetuning
    learn.export()  # export for simple inference
    training_tracker.progress = 0.0
    print("Finished training and exported the model.")


class ProgressUpdateCallback(Callback):

    def __init__(self):
        self.progress = 0.0
        self.dataset_size = 1

    def on_batch_end(self, n_epochs, epoch, num_batch, **kwargs):
        self.progress = (epoch * self.dataset_size + num_batch) / (n_epochs * self.dataset_size)


training_tracker = ProgressUpdateCallback()

