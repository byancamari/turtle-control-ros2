#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <memory>
#include <string>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "turtlesim/msg/pose.hpp"

using namespace std::chrono_literals;
using std::placeholders::_1;

class Turtle : public rclcpp::Node
{
public:
    Turtle()
        : Node("turtle"),
          x_(0),
          y_(0),
          received_pose_(false),
          state_(State::IDLE)
    {
        command_subscription_ =
            this->create_subscription<std_msgs::msg::String>(
                "/move_command",
                10,
                std::bind(&Turtle::receive_command, this, _1)
            );

        pose_subscription_ =
            this->create_subscription<turtlesim::msg::Pose>(
                "/turtle1/pose",
                10,
                std::bind(&Turtle::receive_pose, this, _1)
            );

        velocity_publisher_ =
            this->create_publisher<geometry_msgs::msg::Twist>(
                "/turtle1/cmd_vel",
                10
            );

        timer_ = this->create_wall_timer(
            50ms,
            std::bind(&Turtle::control_movement, this)
        );

        RCLCPP_INFO(
            this->get_logger(),
            "Tartaruga iniciada na posição lógica (%d, %d)",
            x_,
            y_
        );
    }

private:
    enum class State
    {
        IDLE,
        ROTATING,
        MOVING
    };

    double normalize_angle(double angle)
    {
        return std::atan2(std::sin(angle), std::cos(angle));
    }

    void receive_pose(
        const turtlesim::msg::Pose::SharedPtr message
    )
    {
        current_pose_ = *message;
        received_pose_ = true;
    }

    void receive_command(
        const std_msgs::msg::String::SharedPtr message
    )
    {
        if (!received_pose_)
        {
            RCLCPP_WARN(
                this->get_logger(),
                "A posição do turtlesim ainda não foi recebida."
            );
            return;
        }

        if (state_ != State::IDLE)
        {
            RCLCPP_WARN(
                this->get_logger(),
                "A tartaruga ainda está executando outro comando."
            );
            return;
        }

        const std::string command = message->data;
        constexpr double PI = 3.14159265358979323846;

        target_x_ = current_pose_.x;
        target_y_ = current_pose_.y;

        if (command == "right")
        {
            x_++;
            target_x_ += 1.0;
            target_angle_ = 0.0;
        }
        else if (command == "left")
        {
            x_--;
            target_x_ -= 1.0;
            target_angle_ = PI;
        }
        else if (command == "up")
        {
            y_++;
            target_y_ += 1.0;
            target_angle_ = PI / 2.0;
        }
        else if (command == "down")
        {
            y_--;
            target_y_ -= 1.0;
            target_angle_ = -PI / 2.0;
        }
        else
        {
            RCLCPP_WARN(
                this->get_logger(),
                "Comando inválido: %s",
                command.c_str()
            );
            return;
        }

        state_ = State::ROTATING;

        RCLCPP_INFO(
            this->get_logger(),
            "Comando: %s | Nova posição lógica: (%d, %d)",
            command.c_str(),
            x_,
            y_
        );
    }

    void control_movement()
    {
        if (!received_pose_ || state_ == State::IDLE)
        {
            return;
        }

        geometry_msgs::msg::Twist velocity;

        if (state_ == State::ROTATING)
        {
            const double angle_error =
                normalize_angle(target_angle_ - current_pose_.theta);

            if (std::abs(angle_error) > 0.03)
            {
                velocity.angular.z = std::max(
                    -1.5,
                    std::min(1.5, 2.0 * angle_error)
                );
            }
            else
            {
                state_ = State::MOVING;
            }
        }
        else if (state_ == State::MOVING)
        {
            const double delta_x = target_x_ - current_pose_.x;
            const double delta_y = target_y_ - current_pose_.y;
            const double distance = std::hypot(delta_x, delta_y);

            if (distance > 0.03)
            {
                const double desired_angle =
                    std::atan2(delta_y, delta_x);

                const double angle_error =
                    normalize_angle(
                        desired_angle - current_pose_.theta
                    );

                velocity.linear.x = std::min(
                    1.0,
                    std::max(0.15, 1.5 * distance)
                );

                velocity.angular.z = std::max(
                    -1.5,
                    std::min(1.5, 4.0 * angle_error)
                );
            }
            else
            {
                state_ = State::IDLE;

                RCLCPP_INFO(
                    this->get_logger(),
                    "Movimento concluído."
                );
            }
        }

        velocity_publisher_->publish(velocity);
    }

    int x_;
    int y_;

    bool received_pose_;

    double target_x_;
    double target_y_;
    double target_angle_;

    State state_;
    turtlesim::msg::Pose current_pose_;

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr
        command_subscription_;

    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr
        pose_subscription_;

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr
        velocity_publisher_;

    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Turtle>());
    rclcpp::shutdown();

    return 0;
}