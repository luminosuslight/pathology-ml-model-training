from fastai.vision import Callback


class ProgressUpdateCallback(Callback):

    def __init__(self):
        self.progress = 0.0
        self.dataset_size = 1

    def on_batch_end(self, n_epochs, epoch, num_batch, **kwargs):
        self.progress = (epoch * self.dataset_size + num_batch) / (n_epochs * self.dataset_size)


training_tracker = ProgressUpdateCallback()
