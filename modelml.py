from google.colab import drive.mount('/content/drive')
import tensorflow as tf
from tensorflow.keras.preprocessing.image import ImageDataGenerator
import os

base_dir = '/content/drive/MyDrive/DATASET'

train_dir = os.path.join(base_dir, 'TRAIN')
test_dir = os.path.join(base_dir, 'TEST')

img_size = (128, 128)
batch_size = 32

train_datagen = ImageDataGenerator(rescale=1./255, rotation_range=20,
                                   width_shift_range=0.2, height_shift_range=0.2,
                                   shear_range=0.2, zoom_range=0.2, horizontal_flip=True)

test_datagen = ImageDataGenerator(rescale=1./255)

# train_generator = train_datagen.flow_from_directory(
#     train_dir,
#     target_size=img_size,
#     batch_size=batch_size,
#     class_mode='binary'
# )

# test_generator = test_datagen.flow_from_directory(
#     test_dir,
#     target_size=img_size,
#     batch_size=batch_size,
#     class_mode='binary'
# )
train_generator = train_datagen.flow_from_directory(
    '/content/drive/MyDrive/DATASET/TRAIN',
    target_size=(64, 64),
    batch_size=32,
    class_mode='binary',
    shuffle=True,
    subset='training'
)

test_generator = test_datagen.flow_from_directory(
    '/content/drive/MyDrive/DATASET/TEST',
    target_size=(64, 64),
    batch_size=32,
    class_mode='binary',
    shuffle=False
)

# model = tf.keras.models.Sequential([
#     tf.keras.layers.Conv2D(32, (3, 3), activation='relu', input_shape=(128, 128, 3)),
#     tf.keras.layers.MaxPooling2D(2, 2),

#     tf.keras.layers.Conv2D(64, (3, 3), activation='relu'),
#     tf.keras.layers.MaxPooling2D(2, 2),

#     tf.keras.layers.Conv2D(128, (3, 3), activation='relu'),
#     tf.keras.layers.MaxPooling2D(2, 2),

#     tf.keras.layers.Flatten(),
#     tf.keras.layers.Dense(128, activation='relu'),
#     tf.keras.layers.Dropout(0.5),
#     tf.keras.layers.Dense(1, activation='sigmoid')
# ])
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Conv2D, MaxPooling2D, Flatten, Dense, Dropout

model = tf.keras.models.Sequential([
    tf.keras.layers.Conv2D(32, (3, 3), activation='relu', input_shape=(64, 64, 3)),
    tf.keras.layers.MaxPooling2D(2, 2),

    tf.keras.layers.Conv2D(64, (3, 3), activation='relu'),
    tf.keras.layers.MaxPooling2D(2, 2),

    tf.keras.layers.Conv2D(128, (3, 3), activation='relu'),
    tf.keras.layers.MaxPooling2D(2, 2),

    tf.keras.layers.Flatten(),
    tf.keras.layers.Dense(128, activation='relu'),
    tf.keras.layers.Dropout(0.5),
    tf.keras.layers.Dense(1, activation='sigmoid')
])

model.compile(optimizer='adam',
               loss='binary_crossentropy',
               metrics=['accuracy'])

# history = model.fit(
#     train_generator,
#     epochs=10,
#     validation_data=test_generator
# )

history = model.fit(
    train_generator,
    epochs=10,
    steps_per_epoch=50,
    validation_steps=10,
    validation_data=test_generator
)

loss, accuracy = model.evaluate(test_generator)
print(f"Test Accuracy: {accuracy * 100:.2f}%")

model.save('garbage_classifier_model.keras')

from tensorflow.keras.models import load_model
model = load_model('/content/garbage_classifier_model.keras')

# import matplotlib.pyplot as plt
# import numpy as np
# from tensorflow.keras.preprocessing import image
# test_images = [
#     '/content/drive/MyDrive/dataset/test/O/O_12589.jpg',
#     '/content/drive/MyDrive/dataset/test/R/R_10183.jpg',
#     '/content/drive/MyDrive/dataset/test/O/O_13709.jpg',
#     '/content/drive/MyDrive/dataset/test/R/R_10997.jpg',
# ]
# def predict_images(image_paths, model):
#     plt.figure(figsize=(12, len(image_paths) * 2))

#     for i, img_path in enumerate(image_paths):

#         img = image.load_img(img_path, target_size=(128, 128))
#         img_array = image.img_to_array(img) / 255.0
#         img_array = np.expand_dims(img_array, axis=0)


#         pred = model.predict(img_array)[0][0]
#         label = 'Recyclable (R)' if pred > 0.5 else 'Organic (O)'
#         confidence = pred if pred > 0.5 else 1 - pred


#         plt.subplot(len(image_paths), 1, i + 1)
#         plt.imshow(img)
#         plt.axis('off')
#         plt.title(f"{label} — Confidence: {confidence:.2f}", fontsize=14)

#     plt.tight_layout()
#     plt.show()
import matplotlib.pyplot as plt
import numpy as np
from tensorflow.keras.preprocessing import image

test_images = [
    '/content/drive/MyDrive/DATASET/TEST/O/O_12589.jpg',
    '/content/drive/MyDrive/DATASET/TEST/R/R_10183.jpg',
    '/content/drive/MyDrive/DATASET/TEST/O/O_13709.jpg',
    '/content/drive/MyDrive/DATASET/TEST/R/R_10997.jpg',
]

def predict_images(image_paths, model):
    plt.figure(figsize=(12, len(image_paths) * 2))

    for i, img_path in enumerate(image_paths):
        print(f"Loading: {img_path}")

        img = image.load_img(img_path, target_size=(128, 128))
        img_array = image.img_to_array(img) / 255.0
        img_array = np.expand_dims(img_array, axis=0)

        pred = model.predict(img_array)[0][0]
        label = 'Recyclable (R)' if pred > 0.5 else 'Organic (O)'
        confidence = pred if pred > 0.5 else 1 - pred

        print(f"Prediction: {label}, Confidence: {confidence:.2f}")

        plt.subplot(len(image_paths), 1, i + 1)
        plt.imshow(img)
        plt.axis('off')
        plt.title(f"{label} — Confidence: {confidence:.2f}", fontsize=14)

    plt.tight_layout()
    plt.show()

predict_images(test_images, model)
