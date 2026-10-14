import rclpy
from rclpy.node import Node
from status_interfaces.msg import SystemStatus
import psutil
import platform

class StatusPubNode(Node):
    def __init__(self):
        super().__init__('status_pub')
        self.publisher_ = self.create_publisher(SystemStatus, 'system_status', 10)
        self.timer_ = self.create_timer(3.0, self.publish_status)
        
    def publish_status(self):
        msg = SystemStatus()
        msg.stamp = self.get_clock().now().to_msg()
        msg.host_name = platform.node()
        msg.cpu_percent = psutil.cpu_percent()
        memory_info = psutil.virtual_memory()
        msg.memory_percent = memory_info.percent
        msg.memory_total = memory_info.total / 1024 / 1024
        msg.memory_available = memory_info.available / 1024 / 1024
        net_io_info = psutil.net_io_counters()
        msg.net_sent = net_io_info.bytes_sent / 1024 / 1024
        msg.net_recv = net_io_info.bytes_recv / 1024 / 1024
        self.get_logger().info(f'Published SystemStatus message: {str(msg)}')
        self.publisher_.publish(msg)

def main():
    rclpy.init()
    node = StatusPubNode()
    rclpy.spin(node)
    rclpy.shutdown()