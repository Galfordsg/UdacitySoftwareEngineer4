#include "ros/ros.h"
#include "sensor_msgs/Image.h"
#include "ball_chaser/DriveToTarget.h"

ros::ServiceClient client;

void drive_robot(float lin_x, float ang_z)
{
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (client.call(srv))
    {
        ROS_INFO("Drive command sent: linear_x=%f angular_z=%f", lin_x, ang_z);
    }
    else
    {
        ROS_ERROR("Failed to call service command_robot");
    }
}

void process_image_callback(const sensor_msgs::Image img)
{
    int img_width = img.width;

    int left_white = 0;
    int center_white = 0;
    int right_white = 0;

    for (int i = 0; i < img.data.size(); i += 3)
    {
        if (img.data[i] > 200 && img.data[i + 1] > 200 && img.data[i + 2] > 200)
        {
            int pixel_index = i / 3;
            int column = pixel_index % img_width;

            if (column < img_width / 3)
            {
                left_white++;
            }
            else if (column < 2 * img_width / 3)
            {
                center_white++;
            }
            else
            {
                right_white++;
            }
        }
    }

    if (left_white > 0)
    {
        ROS_INFO("White ball detected on LEFT");
        drive_robot(0.0, 0.5);
    }
    else if (center_white > 0)
    {
        ROS_INFO("White ball detected at CENTER");
        drive_robot(0.5, 0.0);
    }
    else if (right_white > 0)
    {
        ROS_INFO("White ball detected on RIGHT");
        drive_robot(0.0, -0.5);
    }
    else
    {
        ROS_INFO("No white ball detected");
        drive_robot(0.0, 0.0);
    }
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    ros::spin();
    return 0;
}
