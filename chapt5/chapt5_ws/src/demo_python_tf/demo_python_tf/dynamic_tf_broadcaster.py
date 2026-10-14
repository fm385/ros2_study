import math
import rclpy
from rclpy.node import Node
from tf2_ros import TransformBroadcaster
from geometry_msgs.msg import TransformStamped
from tf_transformations import quaternion_from_euler

class DynamicTFBroadcaster(Node):
    def __init__(self):
        super().__init__('dynamic_tf2_broadcaster')
        self.broadcaster_ = TransformBroadcaster(self)
        self.timer_ = self.create_timer(3.0, self.publish_transform)  # Publish every second


    def publish_transform(self):
        # Create a TransformStamped message
        t = TransformStamped()

        t.header.stamp = self.get_clock().now().to_msg()
        t.header.frame_id = 'camera_link'
        t.child_frame_id = 'bottle_link'
        t.transform.translation.x = 0.2
        t.transform.translation.y = 0.0
        t.transform.translation.z = 0.5
        rotation_quat = quaternion_from_euler(0, 0, 0)
        t.transform.rotation.x = rotation_quat[0]
        t.transform.rotation.y = rotation_quat[1]
        t.transform.rotation.z = rotation_quat[2]
        t.transform.rotation.w = rotation_quat[3]

        self.broadcaster_.sendTransform(t)
        self.get_logger().info('Publishing dynamic transform from camera_link to bottle_link, TF: {}'.format(t))

def main():
    rclpy.init()
    node = DynamicTFBroadcaster()
    rclpy.spin(node)
    rclpy.shutdown()