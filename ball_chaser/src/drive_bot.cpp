#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "ball_chaser/DriveToTarget.h"

ros::Publisher motor_command_publisher;

bool handle_drive_request(ball_chaser::DriveToTarget::Request &req,
                          ball_chaser::DriveToTarget::Response &res)
{
    geometry_msgs::Twist motor_command;
    motor_command.linear.x = req.linear_x;
    motor_command.angular.z = req.angular_z;

    motor_command_publisher.publish(motor_command);

    res.msg_feedback = req.linear_x + req.angular_z;

    ROS_INFO("Drive command received - linear_x: %f angular_z: %f",
             req.linear_x, req.angular_z);

    return true;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "drive_bot");
    ros::NodeHandle n;

    motor_command_publisher = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
    ros::ServiceServer service = n.advertiseService("/ball_chaser/command_robot", handle_drive_request);

    ROS_INFO("Ready to send drive commands");
    ros::spin();

    return 0;
}
