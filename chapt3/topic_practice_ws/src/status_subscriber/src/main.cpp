#include <QApplication>
#include <QLabel>
#include <QString>
#include <rclcpp/rclcpp.hpp>
#include <status_interfaces/msg/system_status.hpp>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    rclcpp::init(argc, argv);

    QLabel* label = new QLabel();
    label->setAttribute(Qt::WA_DeleteOnClose);
    label->setAlignment(Qt::AlignCenter);
    label->setMinimumSize(400, 300);
    label->show();

    auto node = std::make_shared<rclcpp::Node>("status_subscriber");
    auto subscription = node->create_subscription<status_interfaces::msg::SystemStatus>(
        "system_status", 10,
        [label](const status_interfaces::msg::SystemStatus::SharedPtr msg) {
            std::stringstream ss;
            ss << "================== system_status ==================\n";
            ss << "stamp: " << msg->stamp.sec << "." << msg->stamp.nanosec << "\n";
            ss << "host_name: " << msg->host_name << "\n";
            ss << "cpu_percent: " << msg->cpu_percent << "\n";
            ss << "memory_percent: " << msg->memory_percent << "\n";
            ss << "memory_total: " << msg->memory_total << "\n";
            ss << "memory_available: " << msg->memory_available << "\n";
            ss << "net_sent: " << msg->net_sent << "\n";
            ss << "net_recv: " << msg->net_recv << "\n";
            ss << "====================================================\n";
            label->setText(QString::fromStdString(ss.str()));
        }
    );
    std::thread rclcpp_thread([node]() {
        rclcpp::spin(node);
    });
    rclcpp_thread.detach();

    app.exec();
    rclcpp::shutdown();

    return 0;
}