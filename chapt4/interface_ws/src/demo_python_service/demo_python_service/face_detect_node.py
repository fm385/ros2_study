import rclpy
from rclpy.node import Node
import cv2
import face_recognition
from ament_index_python.packages import get_package_share_directory
from human_recongnize_interface.srv import FaceDecetor
from sensor_msgs.msg import RegionOfInterest
from cv_bridge import CvBridge
import time
from rcl_interfaces.msg import SetParametersResult

class FaceDetectNode(Node):
    def __init__(self):
        super().__init__('face_detect_node')
        self.srv_ = self.create_service(FaceDecetor, 'face_detect', self.face_detect_callback)
        self.bridge_ = CvBridge()
        self.default_image_path_ = get_package_share_directory('demo_python_service') + '/resource/default.jpeg'
        self.declare_parameter('upsample_times', 1)
        self.declare_parameter('model', 'hog')
        self.upsample_times_ = self.get_parameter('upsample_times').value
        self.model_ = self.get_parameter('model').value
        self.add_on_set_parameters_callback(self.parameter_callback)
        self.get_logger().info('Face Detect Service is ready.')


    def parameter_callback(self, params):
        for param in params:
            if param.name == 'upsample_times':
                self.upsample_times_ = param.value
                self.get_logger().info(f'Updated upsample_times to {self.upsample_times_}')
            elif param.name == 'model':
                self.model_ = param.value
                self.get_logger().info(f'Updated model to {self.model_}')
        return SetParametersResult(successful=True)

    def face_detect_callback(self, request, response):
        if request.image.data:
            cv_image = self.bridge_.imgmsg_to_cv2(request.image)
        else:
            cv_image = cv2.imread(self.default_image_path_)

        start_time = time.time()
        face_locations = face_recognition.face_locations(cv_image, number_of_times_to_upsample=self.upsample_times_, model=self.model_)
        end_time = time.time()
        self.get_logger().info(f'Face detection took {end_time - start_time:.4f} seconds.')
        response.face_num = len(face_locations)
        response.use_time = end_time - start_time
        for (top, right, bottom, left) in face_locations:
            face = RegionOfInterest(x_offset=left, y_offset=top, height=bottom-top, width=right-left, do_rectify=False)
            response.faces.append(face)
        return response
    

def main():
    rclpy.init()
    node = FaceDetectNode()
    rclpy.spin(node)
    rclpy.shutdown()