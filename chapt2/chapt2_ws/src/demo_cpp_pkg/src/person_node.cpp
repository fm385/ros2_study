#include <rclcpp/rclcpp.hpp>
#include <string>

class PersonNode : public rclcpp::Node
{
public:
    PersonNode(const std::string& node_name, const std::string& name, const int age)
        : Node(node_name), name_(name), age_(age)
    {
        RCLCPP_INFO(this->get_logger(), "PersonNode '%s' created with name: %s, age: %d", node_name.c_str(), name_.c_str(), age_);
    }

    ~PersonNode()
    {
        RCLCPP_INFO(this->get_logger(), "PersonNode '%s' destroyed.", this->get_name());
    }

    void introduce() const
    {
        RCLCPP_INFO(this->get_logger(), "Hello, my name is %s and I am %d years old.", name_.c_str(), age_);
    }
private:
    std::string name_;
    int age_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

    auto person_node = std::make_shared<PersonNode>("person_node", "Alice", 30);
    person_node->introduce();

    rclcpp::spin(person_node);
    rclcpp::shutdown();
    return 0;
}