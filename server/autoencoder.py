from fastai.vision import ImageImageList, open_image, Image
from torch import nn


class Autoencoder(nn.Module):
    def __init__(self):
        super(Autoencoder, self).__init__()

        self.encoder = nn.Sequential(
            nn.Conv2d(3, 16, kernel_size=3, padding=1),
            nn.ReLU(True),
            nn.MaxPool2d(2, 2),
            nn.Conv2d(16, 8, kernel_size=3, padding=1),
            nn.ReLU(True),
            nn.MaxPool2d(2, 2),
            nn.Conv2d(8, 4, kernel_size=3, padding=1),
            nn.ReLU(True),
            nn.MaxPool2d(2, 2))
        self.decoder = nn.Sequential(
            nn.ConvTranspose2d(4, 8, kernel_size=2, stride=2),
            nn.ReLU(True),
            nn.ConvTranspose2d(8, 16, kernel_size=2, stride=2),
            nn.ReLU(True),
            nn.ConvTranspose2d(16, 3, kernel_size=2, stride=2),
            nn.Sigmoid())

    def forward(self, x):
        x = self.encoder(x)
        #print("Shape after bottleneck:", x.shape)
        x = self.decoder(x)
        return x


class CellAutoencoderItemList(ImageImageList):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.current_image_filename = None
        self.current_image = None

    def open(self, item):
        filename, pos = item
        if self.current_image_filename != filename:
            self.current_image = open_image(filename)
            self.current_image_filename = filename
        # pos is (x, y) center position of a cell on the provided image,
        # we will take a 64x64px patch from this location:
        x = int(pos[0])
        y = int(pos[1])
        # note: fast.ai Image dimensions are (c, y, x)
        return Image(self.current_image.data[:, y-32:y+32, x-32:x+32])
