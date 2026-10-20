import rclpy
from geometry_msgs.msg import PoseStamped, Pose
from nav2_simple_commander.robot_navigator import BasicNavigator, TaskResult
from tf2_ros import TransformListener, Buffer
from tf_transformations import euler_from_quaternion, quaternion_from_euler
from rclpy.duration import Duration

class PatrolNode(BasicNavigator):
    def __init__(self, node_name='patrol_node'):
        super().__init__(node_name)
        self.declare_parameter('initial_point', [0.0, 0.0, 0.0])
        self.declare_parameter('target_points', [0.0, 0.0, 0.0, 1.0, 1.0, 1.57])
        self.initial_point_ = self.get_parameter('initial_point').value
        self.target_points_ = self.get_parameter('target_points').value
        self.buffer_ = Buffer()
        self.listener_ = TransformListener(self.buffer_, self)

    def get_pose_by_xyyaw(self, x, y, yaw):
        pose = PoseStamped()
        pose.header.frame_id = 'map'
        pose.header.stamp = self.get_clock().now().to_msg()

        pose.pose.position.x = x
        pose.pose.position.y = y
        quat = quaternion_from_euler(0, 0, yaw)
        pose.pose.orientation.x = quat[0]
        pose.pose.orientation.y = quat[1]
        pose.pose.orientation.z = quat[2]
        pose.pose.orientation.w = quat[3]
        return pose

    def init_robot_pose(self):
        self.initial_point_ = self.get_parameter('initial_point').value
        self.setInitialPose(self.get_pose_by_xyyaw(self.initial_point_[0],
                                                   self.initial_point_[1],
                                                   self.initial_point_[2]))
        self.waitUntilNav2Active()

    def get_target_points(self):
        points = []
        for index in range(int(len(self.target_points_)/3)):
            x = self.target_points_[index*3]
            y = self.target_points_[index*3+1]
            yaw = self.target_points_[index*3+2]
            points.append([x, y, yaw])
            self.get_logger().info(f'parse target point:{index}->({x},{y},{yaw})')
        return points

    def nav_to_pose(self, target_pos):
        self.waitUntilNav2Active()
        self.goToPose(target_pos)
        while not self.isTaskComplete():
            feedback = self.getFeedback()
            self.get_logger().info(f'predict: {Duration.from_msg(feedback.estimated_time_remaining).nanoseconds / 1e9} s arrive.')
            if Duration.from_msg(feedback.navigation_time) > Duration(seconds=600.0):
                self.cancelTask()

        result = self.getResult()
        if result == TaskResult.SUCCEEDED:
            self.get_logger().info('nav success')
        elif result == TaskResult.CANCELED:
            self.get_logger().info('nav canceled')  
        elif result == TaskResult.FAILED:
            self.get_logger().info('nav failed') 
        else:
            self.get_logger().info('nav invalid') 

    def get_current_pose(self):
        while rclpy.ok():
            try:
                tf = self.buffer_.lookup_transform('map', 'base_footprint',
                                                rclpy.time.Time(seconds=0),
                                                rclpy.time.Duration(seconds=1))
                transform = tf.transform
                rotation_euler = euler_from_quaternion([transform.rotation.x,
                                                        transform.rotation.y,
                                                        transform.rotation.z,
                                                        transform.rotation.w])
                self.get_logger().info(f'move:{transform.translation}, quat:{transform.rotation}, euler:{rotation_euler}')
                return transform
            except Exception as e:
                self.get_logger().warn(f'get transform failed:{str(e)}')


def main():
    rclpy.init()
    patrol = PatrolNode()
    patrol.init_robot_pose()

    while rclpy.ok():
        points = patrol.get_target_points()
        for point in points:
            pose_stamped = patrol.get_pose_by_xyyaw(point[0], point[1], point[2])
            patrol.nav_to_pose(pose_stamped)
    rclpy.shutdown()