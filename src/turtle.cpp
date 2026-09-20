#include <memory>
#include <string>
#include <functional>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using std::placeholders::_1;

class Turtle : public rclcpp::Node
{
public:
    Turtle() : Node("turtle"), x_(0), y_(0)
    {
        subscription_ =
            this->create_subscription<std_msgs::msg::String>(
                "/move_command",
                10,
                std::bind(&Turtle::receive_command, this, _1)
            );

        RCLCPP_INFO(
            this->get_logger(),
            "Tartaruga iniciada na posição (%d, %d)",
            x_,
            y_
        );
    }

private:
    void receive_command(
        const std_msgs::msg::String::SharedPtr message
    )
    {
        const std::string command = message->data;

        if (command == "right")
        {
            x_++;
        }
        else if (command == "left")
        {
            x_--;
        }
        else if (command == "up")
        {
            y_++;
        }
        else if (command == "down")
        {
            y_--;
        }
        else
        {
            RCLCPP_WARN(
                this->get_logger(),
                "Comando inválido recebido: %s",
                command.c_str()
            );

            return;
        }

        RCLCPP_INFO(
            this->get_logger(),
            "Comando: %s | Nova posição: (%d, %d)",
            command.c_str(),
            x_,
            y_
        );
    }

    int x_;
    int y_;

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr
        subscription_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    rclcpp::spin(std::make_shared<Turtle>());

    rclcpp::shutdown();

    return 0;
}