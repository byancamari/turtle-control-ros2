#include <iostream>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class SendMsg : public rclcpp::Node
{
public:
    SendMsg() : Node("send_msg")
    {
        publisher_ = this->create_publisher<std_msgs::msg::String>(
            "/move_command",
            10
        );

        RCLCPP_INFO(
            this->get_logger(),
            "Nó send_msg iniciado."
        );
    }

    void send_command(const std::string &command)
    {
        std_msgs::msg::String message;
        message.data = command;

        publisher_->publish(message);

        RCLCPP_INFO(
            this->get_logger(),
            "Comando enviado: %s",
            command.c_str()
        );
    }

private:
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<SendMsg>();

    std::string command;

    while (rclcpp::ok())
    {
        std::cout << "Digite right, left, up, down ou exit: ";
        std::cin >> command;

        if (command == "exit")
        {
            break;
        }

        if (
            command == "right" ||
            command == "left" ||
            command == "up" ||
            command == "down"
        )
        {
            node->send_command(command);
        }
        else
        {
            std::cout << "Comando inválido!" << std::endl;
        }

        rclcpp::spin_some(node);
    }

    rclcpp::shutdown();
    return 0;
}