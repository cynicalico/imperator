from PIL import Image

desired_sizes = [
    (32, 32),
    (48, 48),
    (64, 64),
    (96, 96),
    (128, 128),
    (256, 256),
    (384, 384),
    (512, 512),
    (1024, 1024)
]

i = Image.open('png/16x16.png')
for w, h in desired_sizes:
    i.resize((w, h), Image.NEAREST).save(f"png/{w}x{h}.png")
