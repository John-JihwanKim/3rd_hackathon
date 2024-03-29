# Copyright 2017 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os
import argparse
import sys
import numpy as np
import tensorflow as tf
import cv2
from cv2 import cvtColor, drawContours, findContours, imread, rectangle, threshold
import time

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'  # or any {'0', '1', '2'}


def load_graph(model_file):
    graph = tf.Graph()
    graph_def = tf.GraphDef()

    with open(model_file, "rb") as f:
        graph_def.ParseFromString(f.read())
    with graph.as_default():
        tf.import_graph_def(graph_def)

    return graph


def read_tensor_from_image_file(file_name,
                                input_height=299,
                                input_width=299,
                                input_mean=0,
                                input_std=255):
    '''                                
    input_name = "file_reader"
    output_name = "normalized"
    file_reader = tf.read_file(file_name, input_name)
    if file_name.endswith(".png"):
        image_reader = tf.image.decode_png(file_reader, channels=3, name="png_reader")
    elif file_name.endswith(".gif"):
        image_reader = tf.squeeze(tf.image.decode_gif(file_reader, name="gif_reader"))
    elif file_name.endswith(".bmp"):
        image_reader = tf.image.decode_bmp(file_reader, name="bmp_reader")
    else:
        #image_reader = tf.image.decode_jpeg(file_reader, channels=3, name="jpeg_reader")
        
        #img2 = cv2.imread(file_name)
        #img2 = cv2.resize(img2, dsize = (227, 227), interpolation = cv2.INTER_CUBIC)
        #np_image_data = np.asarray(img2)
        #np_image_data = cv2.normalize(img2.astype('float'), None, -0.5, 0.5, cv2.NORM_MINMAX)        
        #image_reader = np.expand_dims(np_image_data, axis=0)
    '''    
        
    #img2 = cv2.imread(file_name)
    
    image_reader = cv2.cvtColor(file_name, cv2.COLOR_BGR2RGB)
    image_reader = file_name.astype(np.float32)

    #image_reader = image_reader[100:800, 100:1000]
        
    float_caster = tf.cast(image_reader, tf.float32)
    dims_expander = tf.expand_dims(float_caster, 0)
    resized = tf.image.resize_bilinear(dims_expander, [input_height, input_width])
    normalized = tf.divide(tf.subtract(resized, [input_mean]), [input_std])
    sess = tf.Session()
    result = sess.run(normalized)

    return result


def load_labels(label_file):
    label = []
    proto_as_ascii_lines = tf.gfile.GFile(label_file).readlines()
    for l in proto_as_ascii_lines:
        label.append(l.rstrip())
    return label


if __name__ == "__main__":    
    #file_name = sys.argv[2]
    #model_file = "food_graph.pb"  
    #model_file = "output_graph.pb"  
    model_file = "final_graph.pb"  
    #model_file = "converted_model.tflite"  
    
    label_file = "output_labels.txt"
    #input_height = 299
    #input_width = 299
    input_height = 224     #for mobile graph
    input_width = 224
    input_mean = 0
    input_std = 255
    #input_layer = "input"
    input_layer = "Placeholder"
    #output_layer = "InceptionV3/Predictions/Reshape_1"
    output_layer = "final_result"

    parser = argparse.ArgumentParser()
    parser.add_argument("--image", help="image to be processed")
    parser.add_argument("--graph", help="graph/model to be executed")
    parser.add_argument("--labels", help="name of file containing labels")
    parser.add_argument("--input_height", type=int, help="input height")
    parser.add_argument("--input_width", type=int, help="input width")
    parser.add_argument("--input_mean", type=int, help="input mean")
    parser.add_argument("--input_std", type=int, help="input std")
    parser.add_argument("--input_layer", help="name of input layer")
    parser.add_argument("--output_layer", help="name of output layer")
    args = parser.parse_args()

    if args.graph:
        model_file = args.graph
    if args.image:
        file_name = args.image
    if args.labels:
        label_file = args.labels
    if args.input_height:
        input_height = args.input_height
    if args.input_width:
        input_width = args.input_width
    if args.input_mean:
        input_mean = args.input_mean
    if args.input_std:
        input_std = args.input_std
    if args.input_layer:
        input_layer = args.input_layer
    if args.output_layer:
        output_layer = args.output_layer

        
    graph = load_graph(model_file)

    #print([n.name for n in tf.get_default_graph().as_graph_def().node])
    
    cap = cv2.VideoCapture(0)
    #fps = cap.get(cv2.CAP_PROP_FPS)
    #print ("Frames per second: {0}".format(fps))
    #cap.set(cv2.CAP_PROP_FPS, 30)
    print ('Starting image capture')

    running = True
    while (running):
        try:
            #time.sleep(0.1)	
            start = time.time()
            ret, frame = cap.read()   
            u_label = '.'
            b_label = '.'
            if ret:
                '''    
                -------------------------------------------
                |                                         | 
                |    (x1, y1)      w                      |
                |      ------------------------           |
                |      |                      |           |
                |      |                      |           | 
                |      |         ROI          | h         | 
                |      |                      |           | 
                |      |                      |           | 
                |      |                      |           | 
                |      ------------------------           | 
                |                           (x2, y2)      | 
                |                                         | 
                |                                         | 
                |                                         | 
                -------------------------------------------             
                ROI = image[y1:y2, x1:x2]
                '''
                height, width, _= frame.shape
                
                u_frame = frame[0:int(height/2), 0:width]  # ROI for upper in the frame 
                b_frame = frame[int(height/2):height, 0:width]  # ROI for bottom in the frame 
                
                t = read_tensor_from_image_file(
                    u_frame,
                    input_height=input_height,
                    input_width=input_width,
                    input_mean=input_mean,
                    input_std=input_std)

                input_name = "import/" + input_layer
                output_name = "import/" + output_layer
                input_operation = graph.get_operation_by_name(input_name)
                output_operation = graph.get_operation_by_name(output_name)

                with tf.Session(graph=graph) as sess:
                    results = sess.run(output_operation.outputs[0], {
                        input_operation.outputs[0]: t
                    })
                results = np.squeeze(results)

                top_k = results.argsort()[-5:][::-1]
                labels = load_labels(label_file)
                #print("state: %s, score: %d" % (labels[top_k[0]], results[top_k[0]] * 100))
                if(labels[top_k[0]] == 'meat' and ((results[top_k[0]] * 100) > 50)):
                    print("meat detected (%d)" % (results[top_k[0]] * 100))
                elif(labels[top_k[0]] == 'pot' and ((results[top_k[0]] * 100) > 50)):
                    print('pot detected', ((results[top_k[0]] * 100) > 50)))
                elif(labels[top_k[0]] == 'shrimp' and ((results[top_k[0]] * 100) > 50)):
                    print('shrimp detected', ((results[top_k[0]] * 100) > 50)))                    
                #else:
                    #print('cooking other food')
                #    print('.')

                u_label = labels[top_k[0]]
                #cv2.imshow("captured image", u_frame)
                
                t = read_tensor_from_image_file(
                    b_frame,
                    input_height=input_height,
                    input_width=input_width,
                    input_mean=input_mean,
                    input_std=input_std)

                input_name = "import/" + input_layer
                output_name = "import/" + output_layer
                input_operation = graph.get_operation_by_name(input_name)
                output_operation = graph.get_operation_by_name(output_name)

                with tf.Session(graph=graph) as sess:
                    results = sess.run(output_operation.outputs[0], {
                        input_operation.outputs[0]: t
                    })
                results = np.squeeze(results)

                top_k = results.argsort()[-5:][::-1]
                labels = load_labels(label_file)
                #print("state: %s, score: %d" % (labels[top_k[0]], results[top_k[0]] * 100))
                if(labels[top_k[0]] == 'meat' and ((results[top_k[0]] * 100) > 50)):
                    print("meat detected (%d)" % (results[top_k[0]] * 100))
                elif(labels[top_k[0]] == 'pot' and ((results[top_k[0]] * 100) > 50)):
                    print('pot detected')
                elif(labels[top_k[0]] == 'shrimp' and ((results[top_k[0]] * 100) > 50)):
                    print('shrimp detected')  
                #else:
                    #print('cooking other food')
                #    print('.')

                b_label = labels[top_k[0]]
                #cv2.imshow("captured image2", b_frame)                
                
                imgs_comb = np.vstack((u_frame, b_frame))
                cv2.imshow("final image", imgs_comb)
                #key = cv2.waitKey(1000)
                key = cv2.waitKey(500)
                                
                cv2.destroyAllWindows()
                #key = cv2.waitKey(0)
                #if(key == 27): 
                #    cv2.destroyAllWindows()
                    
            else:
                print ('no more frame')
                break

            
            print(u_label, b_label)
            end = time.time()
            seconds = end - start
            #print ("Time taken : {0} seconds".format(seconds))
        except KeyboardInterrupt:
            #print('key interrupt, quit')
            break
                
    print('Exiting, release cam')
    cap.release()
    cv2.destroyAllWindows()
	
