import math
import rclpy
from rclpy.node import Node
from tf2_ros import StaticTransformBroadcaster
from geometry_msgs.msg import TransformStamped
from tf_transformations import quaternion_from_euler

class StaticTFBroadcaster(Node):
    def __init__(self):
        super().__init__('static_tf2_broadcaster')
        self.broadcaster_ = StaticTransformBroadcaster(self)

        # Create a TransformStamped message
        t = TransformStamped()

        t.header.stamp = self.get_clock().now().to_msg()
        t.header.frame_id = 'base_link'
        t.child_frame_id = 'camera_link'
        t.transform.translation.x = 0.5
        t.transform.translation.y = 0.3
        t.transform.translation.z = 0.6
        rotation_quat = quaternion_from_euler(math.radians(180), 0, 0)
        t.transform.rotation.x = rotation_quat[0]
        t.transform.rotation.y = rotation_quat[1]
        t.transform.rotation.z = rotation_quat[2]
        t.transform.rotation.w = rotation_quat[3]

        self.broadcaster_.sendTransform(t)
        self.get_logger().info('Publishing static transform from base_link to camera_link, TF: {}'.format(t))

def main():
    rclpy.init()
    node = StaticTFBroadcaster()
    rclpy.spin(node)
    rclpy.shutdown()