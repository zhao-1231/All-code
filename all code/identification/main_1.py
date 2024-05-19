#%%
import tensorflow as tf
from tensorflow import keras
from keras import layers, Input, Model
import numpy as np
import matplotlib.pyplot as plt
#%%
# data loader
ImageRootPath = r'/home/featurize/data/Raw Image Dataset'
IMG_SIZE = (224,224)
BATCH_SIZE = 32
rng = tf.random.Generator.from_seed(245,alg='philox')

train_dataset = tf.keras.preprocessing.image_dataset_from_directory(
    directory=ImageRootPath,
    image_size=IMG_SIZE,
    batch_size=BATCH_SIZE,
    subset="training",
    seed = 861,
    validation_split=0.7
)

test_dataset = tf.keras.preprocessing.image_dataset_from_directory(
    directory=ImageRootPath,
    image_size=IMG_SIZE,
    batch_size=BATCH_SIZE,
    subset="validation",
    seed=861,
    validation_split=0.7
)
#%%
# build model
def data_aug(images,labels):
    seed = rng.make_seeds(2)[0]
    augted_image = tf.image.stateless_random_brightness(images,25,seed)
    augted_image = tf.image.stateless_random_contrast(augted_image,0.7,1.4,seed)
    augted_image = tf.image.stateless_random_flip_left_right(augted_image,seed)
    augted_image = tf.image.stateless_random_flip_up_down(augted_image,seed)
    return augted_image,labels

class CustomModel(Model):
    def train_step(self, data):
        x,y = data
        x,y = data_aug(x,y)
        with tf.GradientTape() as tape:
            y_pred = self(x,training=True)
            loss = self.compiled_loss(y,y_pred,regularization_losses=self.losses)
        trainable_vars = self.trainable_variables
        gradients = tape.gradient(loss,trainable_vars)
        self.optimizer.apply_gradients(zip(gradients,trainable_vars))
        self.compiled_metrics.update_state(y,y_pred)
        return {m.name:m.result() for m in self.metrics}

    def test_step(self, data):
        x,y = data
        y_pred = self(x,training=False)
        self.compiled_loss(y, y_pred, regularization_losses=self.losses)
        self.compiled_metrics.update_state(y,y_pred)
        return {m.name: m.result() for m in self.metrics}

def RepVggBlock(inputs,filters,stride=2):
    Conv3X = layers.Conv2D(filters=filters,kernel_size=(3,3),strides=stride,padding='same',use_bias=False)(inputs)
    Conv3X = layers.BatchNormalization()(Conv3X)
    Conv1X = layers.Conv2D(filters=filters,kernel_size=(1,1),strides=stride,padding='same',use_bias=False)(inputs)
    Conv1X = layers.BatchNormalization()(Conv1X)   
    ConvRes = layers.Add()([Conv3X,Conv1X])
    ConvRes = layers.ReLU()(ConvRes)

    Conv3X = layers.Conv2D(filters=filters,kernel_size=(3,3),strides=1,padding='same',use_bias=False)(ConvRes)
    Conv3X = layers.BatchNormalization()(Conv3X)
    Conv1X = layers.Conv2D(filters=filters,kernel_size=(1,1),strides=1,padding='same',use_bias=False)(ConvRes)
    Conv1X = layers.BatchNormalization()(Conv1X) 
    identity = layers.BatchNormalization()(ConvRes) 
    ConvRes = layers.Add()([Conv3X,Conv1X,identity])
    ConvRes = layers.ReLU()(ConvRes)

    Conv3X = layers.Conv2D(filters=filters,kernel_size=(3,3),strides=1,padding='same',use_bias=False)(ConvRes)
    Conv3X = layers.BatchNormalization()(Conv3X)
    Conv1X = layers.Conv2D(filters=filters,kernel_size=(1,1),strides=1,padding='same',use_bias=False)(ConvRes)
    Conv1X = layers.BatchNormalization()(Conv1X) 
    identity = layers.BatchNormalization()(ConvRes) 
    ConvRes = layers.Add()([Conv3X,Conv1X,identity])
    ConvRes = layers.ReLU()(ConvRes)    

    Conv3X = layers.Conv2D(filters=filters,kernel_size=(3,3),strides=1,padding='same',use_bias=False)(ConvRes)
    Conv3X = layers.BatchNormalization()(Conv3X)
    Conv1X = layers.Conv2D(filters=filters,kernel_size=(1,1),strides=1,padding='same',use_bias=False)(ConvRes)
    Conv1X = layers.BatchNormalization()(Conv1X) 
    identity = layers.BatchNormalization()(ConvRes) 
    ConvRes = layers.Add()([Conv3X,Conv1X,identity])
    ConvRes = layers.ReLU()(ConvRes)   

    return ConvRes

def AttentionLayer(inputs,ks = 1,out_cls = 7,decay_weight=-1.):
    # normed_inputs = layers.BatchNormalization()(inputs)
    attention = layers.Conv2D(filters=out_cls * 7,kernel_size=ks,strides=1,padding='same',use_bias=False,activation='relu')(inputs)
    attention = layers.Conv2D(filters=out_cls,kernel_size=ks,strides=1,padding='same',use_bias=False,activation='softmax')(attention)
    attention = tf.math.multiply(attention,tf.constant([[decay_weight,1.,1.,1.,1.,1.,1.]],dtype=tf.float32))
    attention = tf.reduce_sum(attention,axis=-1,keepdims=True)
    attention = layers.ReLU()(attention) * 2.
    outputs = tf.math.multiply(inputs,attention)
    return outputs


def MakeConvModel(name="RepVgg"):
    inputs = Input(shape=(224,224,3),dtype=tf.float32)
    inputs = layers.Rescaling(scale=1./255)(inputs)
    s1 = RepVggBlock(inputs,16) # shape [112,112]
    s2 = RepVggBlock(s1,24) # shape [56,56]
    s3 = RepVggBlock(s2,24) # shape [28,28]
    # s3 = AttentionLayer(s3,ks=3,decay_weight=-1.)
    s4 = RepVggBlock(s3,32) # shape [14,14]
    s5 = RepVggBlock(s4,32) # shape [7,7]
    feature = layers.GlobalAveragePooling2D()(s5)
    outputs = layers.Dense(7,activation='softmax')(feature)
    return CustomModel(inputs=inputs,outputs=outputs,name=name)
#%%
model = MakeConvModel()
model.summary()
initial_learning_rate = 1e-3
modelRecordPath = r'/home/featurize/data/SavedModel'
decay_steps = 200
lr_decayed_fn = tf.keras.optimizers.schedules.CosineDecay(
    initial_learning_rate, decay_steps)
#checkPointCallBack = tf.keras.callbacks.ModelCheckpoint(filepath=modelRecordPath,monitor='val_accuracy',\
#    mode='max',save_best_only=True,)
# class_weight = {0:1,1:1,2:1,3:1,4:1,5:0.7,6:1.2}

lr_callback = tf.keras.callbacks.LearningRateScheduler(lr_decayed_fn) 
optimizer = keras.optimizers.Adam(learning_rate=initial_learning_rate)
loss = keras.losses.SparseCategoricalCrossentropy(from_logits=False)
model.compile(optimizer=optimizer,loss=loss,metrics='accuracy')
model.fit(train_dataset,validation_data=test_dataset,epochs=200,callbacks=[lr_callback])
#%%



