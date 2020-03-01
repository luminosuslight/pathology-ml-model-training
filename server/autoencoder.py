#from unet_autoencoder import unet_autoencoder_learner
from vae_resnet18_autoencoder import VAE
from train import training_tracker

from fastai.vision import *
from fastai.callbacks import *

from pathlib import Path
import os
import shutil


class Autoencoder(nn.Module):
    def __init__(self):
        super(Autoencoder, self).__init__()

        self.encoder = nn.Sequential(
            nn.Conv2d(3, 6, kernel_size=5),
            nn.ReLU(True),
            nn.Conv2d(6, 16, kernel_size=5),
            nn.ReLU(True))
        self.decoder = nn.Sequential(
            nn.ConvTranspose2d(16, 6, kernel_size=5),
            nn.ReLU(True),
            nn.ConvTranspose2d(6, 3, kernel_size=5),
            nn.ReLU(True))

    def forward(self, x):
        x = self.encoder(x)
        x = self.decoder(x)
        return x


class CellAutoencoderItemList(ImageImageList):

    def open(self, item):
        filename, pos = item
        if not hasattr(self, 'current_filename') or self.current_filename != filename:
            self.current_filename = filename
            self.whole_image = open_image(filename)
        # pos is (x, y) center position of a cell on the provided image
        # we will take a 64x64px patch from this location:
        x = int(pos[0])
        y = int(pos[1])
        # fast.ai Image dims are (c, y, x)
        return Image(self.whole_image.data[:, y-32:y+32, x-32:x+32])


def prepare_and_train_autoencoder(params, model_id, img_path):

    model_path = Path("models")/model_id
    if os.path.exists(model_path):
        shutil.rmtree(model_path, ignore_errors=True)

    os.makedirs(model_path)

    if params['baseModel']:
        base_model_weights = Path("models")/params['baseModel']/'trained_model.pth'
    else:
        base_model_weights = ""

    # prepare train data:
    items = [(img_path, pos) for pos in params['cellPositions']]
    item_list = CellAutoencoderItemList(items, path=model_path/'input')
    item_lists = item_list.split_by_rand_pct()
    label_lists = item_lists.label_from_func(lambda x: x, label_cls=CellAutoencoderItemList)
    tfms = ((dihedral(),
             brightness(change=(0.4, 0.6)),
             contrast(scale=(0.9, 1.1)),
             ), [])  # empty list at the end means no transforms for the validation set
    label_lists = label_lists.transform(tfms, tfm_y=True)
    databunch = label_lists.databunch(bs=1, num_workers=1)
    databunch.c = 3

    # TODO: store model name

    train_unet_autoencoder(model_path, base_model_weights, databunch, epochs=params['epochs'])


def train_unet_autoencoder(path, base_model_weights, databunch, epochs):
    training_tracker.dataset_size = len(databunch.train_dl)

    wd = 1e-3
    loss_gen = MSELossFlat()

    arch = models.resnet18
    # learn = unet_autoencoder_learner(databunch, arch, pretrained=True, wd=wd, blur=False, norm_type=NormType.Weight,
    #                      self_attention=True, y_range=None, last_cross=False, loss_func=loss_gen, callbacks=[training_tracker],
    #                      callback_fns=[CSVLogger, partial(EarlyStoppingCallback, monitor='valid_loss', min_delta=0.01, patience=2)])
    model = Autoencoder()
    learn = Learner(databunch, model, loss_func=loss_gen, callbacks=[training_tracker],
                          callback_fns=[CSVLogger, partial(EarlyStoppingCallback, monitor='valid_loss', min_delta=0.01, patience=2)])
    apply_init(model, nn.init.kaiming_normal_)
    # learn = learn.to_fp16()  # to save memory?

    if base_model_weights:
        print("Loading weights of base model...", base_model_weights)
        with open(base_model_weights, 'rb') as file:
            learn.load(file)

    # we want to train both the encoder and decoder, but by default, the pretrained encoder is frozen:
    #learn.unfreeze()

    print(learn.summary())  # TODO: save this to a file

    learning_rate = 1e-4
    print("Training...")
    # at this point, the early layers are frozen and only the last layers are trained
    learn.fit_one_cycle(epochs, learning_rate)
    learn.recorder.plot(return_fig=True).savefig(path/'lr_vs_loss.png')
    learn.recorder.plot_losses(return_fig=True).savefig(path/'losses.png')
    learn.recorder.plot_lr(return_fig=True).savefig(path/'learning_rate.png')
    #learn.recorder.plot_metrics(return_fig=True).savefig(path/'metrics.png')
    print("Saving...")
    with open(path/'trained_model.pth', 'wb') as file:
        learn.save(file)  # save for later finetuning
    learn.export()  # export for simple inference
    training_tracker.progress = 0.0
    print("Finished training and exported the model.")

