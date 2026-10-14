import rclpy
from rclpy.node import Node
import espeakng
from example_interfaces.msg import String
from queue import Queue
import threading
import time

class NovelSubNode(Node):
    def __init__(self, node_name):
        super().__init__(node_name)
        self.get_logger().info(f"Node {node_name} has been started.")
        self.queue_ = Queue()
        self.novel_subscription_ = self.create_subscription(String, 'novel', self.novel_callback, 10)
        self.speak_thread_ = threading.Thread(target=self.speak)
        self.speak_thread_.start()

    def novel_callback(self, msg):
        self.get_logger().info(f"Received: {msg.data}")
        self.queue_.put(msg.data)

    def speak(self):
        speaker = espeakng.Speaker()
        speaker.voice = 'zh'
        while rclpy.ok():
            if not self.queue_.empty():
                line = self.queue_.get()
                self.get_logger().info(f"Speaking: {line}")
                speaker.say(line)
                speaker.wait()  # Wait for the speech to finish before proceeding
            else:
                time.sleep(1)  # Sleep briefly to avoid busy waiting

def main():
    rclpy.init()
    node = NovelSubNode('novel_sub')
    rclpy.spin(node)
    rclpy.shutdown()