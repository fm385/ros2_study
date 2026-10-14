import rclpy
from rclpy.node import Node
import cv2
import face_recognition
from ament_index_python.packages import get_package_share_directory
from human_recongnize_interface.srv import FaceDecetor
from sensor_msgs.msg import RegionOfInterest
from cv_bridge import CvBridge
import time
from rcl_interfaces.srv import SetParameters
from rcl_interfaces.msg import Parameter, ParameterValue, ParameterType

class FaceClientNode(Node):
    def __init__(self):
        super().__init__('face_client_node')
        self.client_ = self.create_client(FaceDecetor, 'face_detect')
        self.defalut_image_path_ = get_package_share_directory('demo_python_service') + '/resource/test1.jpeg'
        self.image_ = cv2.imread(self.defalut_image_path_)
        self.bridge_ = CvBridge()
        self.get_logger().info('Face Client Node is ready.')

    def call_set_parameters(self, parameters):
        client = self.create_client(SetParameters, '/face_detect_node/set_parameters')
        while not client.wait_for_service(timeout_sec=3.0):
            self.get_logger().info('Waiting for set_parameters service...')

        request = SetParameters.Request()
        request.parameters = parameters
        future = client.call_async(request)
        rclpy.spin_until_future_complete(self, future)
        return future.result()

    def update_model(self, model):
        param = Parameter()
        param.name = 'model'
        new_value = ParameterValue()
        new_value.type = ParameterType.PARAMETER_STRING
        new_value.string_value = model
        param.value = new_value
        response = self.call_set_parameters([param])
        for result in response.results:
            if result.successful:
                self.get_logger().info(f'Successfully updated model to {model}')
            else:
                self.get_logger().error(f'Failed to update model: {result.reason}')


    def send_request(self):
        while not self.client_.wait_for_service(timeout_sec=3.0):
            self.get_logger().info('Waiting for service...')

        request = FaceDecetor.Request()
        request.image = self.bridge_.cv2_to_imgmsg(self.image_,)
        future = self.client_.call_async(request)
        # 1. Add a callback function to handle the response
        #future.add_done_callback(self.handle_response)
        # 2. Wait for the response and handle it
        rclpy.spin_until_future_complete(self, future)
        # self.handle_response(future)

    def handle_response(self, future):
        response = future.result()
        if response is not None:
            self.get_logger().info('Received response from service.')
            # Process the response and display the detected face regions
            for i in range(response.face_num):
                rect = response.faces[i]
                x_offset = rect.x_offset
                y_offset = rect.y_offset
                width = rect.width
                height = rect.height
                cv2.rectangle(self.image_, (x_offset, y_offset), (x_offset + width, y_offset + height), (0, 255, 0), 2)
            cv2.imshow('Detected Faces', self.image_)
            cv2.waitKey(0)
            cv2.destroyAllWindows()
        else:
            self.get_logger().error('Service call failed.')


def main():
    rclpy.init()
    node = FaceClientNode()
    node.send_request()
    node.update_model('cnn')  # Update the model parameter to 'cnn'
    node.send_request()  # Send another request with the updated model
    rclpy.spin(node)
    rclpy.shutdown()
    