import rclpy
from rclpy.node import Node
from tf2_ros import TransformListener, Buffer
from tf_transformations import euler_from_quaternion

class TFListenerNode(Node):
    def __init__(self):
        super().__init__('tf_listener_node')
        self.tf_buffer_ = Buffer()
        self.tf_listener_ = TransformListener(self.tf_buffer_, self)
        self.timer_ = self.create_timer(3, self.get_transform)

    def get_transform(self):
        try:
            # Look up the transform from 'base_link' to 'bottle_link'
            result = self.tf_buffer_.lookup_transform('base_link', 'bottle_link', rclpy.time.Time(), rclpy.duration.Duration(seconds=1.0))
            transform = result.transform
            rotation_euler = euler_from_quaternion([transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w])
            self.get_logger().info(f'Move:{transform.translation}, rotation:{transform.rotation}, rotation_euler:{rotation_euler}')
        except Exception as e:
            self.get_logger().warn(f'get Tf failed, e:{e}')


def main():
    rclpy.init()
    node = TFListenerNode()
    rclpy.spin(node)
    rclpy.shutdown()