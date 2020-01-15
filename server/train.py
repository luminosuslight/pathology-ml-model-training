import fastai
from fastai.vision import *
from fastai.callbacks import *
from fastai.utils.mem import GPUMemTrace

from pathlib import Path


def train_unet(path, epochs):
    path = Path(path)
    item_list = ImageImageList.from_folder(path/'input')
    item_lists = item_list.split_by_valid_func(lambda x: x.name.startswith('valid_'))
    label_lists = item_lists.label_from_func(lambda x: path/'target'/x.name)
    databunch = label_lists.databunch(bs=1)
    databunch.c = 3

    wd = 1e-3
    y_range = (-3., 3.)
    loss_gen = MSELossFlat()

    arch = models.resnet18
    learn = unet_learner(databunch, arch, pretrained=True, wd=wd, blur=True, norm_type=NormType.Weight,
                         self_attention=True, y_range=y_range, loss_func=loss_gen, callback_fns=[CSVLogger])
    # learn = learn.to_fp16()  # to save memory?

    lr = 1e-3
    print("starting to train...")
    learn.fit_one_cycle(epochs, lr)
    print("finished training, exporting model...")
    learn.export()
    print("model exported")




