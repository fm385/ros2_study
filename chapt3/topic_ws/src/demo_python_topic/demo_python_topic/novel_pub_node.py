import rclpy
from rclpy.node import Node
import requests
from example_interfaces.msg import String
from queue import Queue

class NovelPubNode(Node):
    def __init__(self, node_name):
        super().__init__(node_name)
        self.get_logger().info(f"Node {node_name} has been started.")
        self.queue_ = Queue()
        self.novel_publisher_ = self.create_publisher(String, 'novel', 10)
        self.timer_ = self.create_timer(5.0, self.timer_callback)  # Timer callback every second

    def timer_callback(self):
        if not self.queue_.empty():
            line = self.queue_.get()
            msg = String(data=line)
            self.novel_publisher_.publish(msg)
            self.get_logger().info(f"Published: {line}")
        else:
            self.novel_publisher_.publish(String(data="No more lines to publish."))
            self.get_logger().info("No more lines to publish.")

    def download(self, url):
        response = requests.get(url)
        response.encoding = 'utf-8'
        self.get_logger().info(f"Downloaded content from {url}: {response.text[:100]}...")  # Log first 100 chars
        for line in response.text.splitlines():
            self.queue_.put(line)

def main():
    rclpy.init()
    node = NovelPubNode('novel_pub')
    node.download('http://0.0.0.0:8000/novel1.txt')
    rclpy.spin(node)
    rclpy.shutdown()