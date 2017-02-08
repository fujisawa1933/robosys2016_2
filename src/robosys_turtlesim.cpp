#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>


ros::Publisher velocity_publisher;
ros::Subscriber pose_subscriber;

geometry_msgs::Twist vel_msg;
turtlesim::Pose turtlesim_pose;

const double PI = 3.14159;

void poseCallback(const turtlesim::Pose::ConstPtr & pose_message);
void move(double distance);
void rotate(double relative_angle);


int main(int argc, char **argv)
{

	ros::init(argc, argv, "turtle_move");
	ros::NodeHandle n;

	velocity_publisher = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);


	pose_subscriber = n.subscribe("/turtle1/pose", 10, poseCallback);


	ROS_INFO("\n************** Start the turtle moving **************\n");

	for(int i = 0; i < 4; i++){
		move(3.8);
		rotate(PI+PI/5);
	}

	move(4);

	ROS_INFO("\n************** Finished the turtle moving **************\n");

	return 0;
}



void poseCallback(const turtlesim::Pose::ConstPtr & pose_message)
{
	turtlesim_pose.x = pose_message -> x;
	turtlesim_pose.y = pose_message -> y;
	turtlesim_pose.theta = pose_message -> theta;
}


void move(double distance)
{

	double pose0_x = 0, pose0_y = 0;
	double pose1_x = 0, pose1_y = 0;
	double current_distance = 0;

	ros::Rate loop_rate(1000);


	do{

		ros::spinOnce();
		pose0_x = turtlesim_pose.x;
		pose0_y = turtlesim_pose.y;

	}while(pose0_x == 0 || pose0_y == 0);


	vel_msg.linear.x = 2;

	do{

		velocity_publisher.publish(vel_msg);

		ros::spinOnce();
		pose1_x = turtlesim_pose.x;
		pose1_y = turtlesim_pose.y;

		if((pose1_x - pose0_x)==0)
			current_distance = pose1_y - pose0_y;
		else if((pose1_y - pose0_y)==0)
			current_distance = pose1_x - pose0_x;
		else
			current_distance = sqrt(pow(pose1_x - pose0_x, 2) + pow(pose1_y - pose0_y, 2));

		loop_rate.sleep();
	}while(fabs(current_distance) < distance);

	vel_msg.linear.x = 0;
	velocity_publisher.publish(vel_msg);
}


void rotate(double target_angle)
{

	ros::Rate loop_rate(1000);
	double current_angle = 0;

	vel_msg.angular.z = 1.0;
	target_angle += turtlesim_pose.theta;
	if(target_angle > 2*PI)
		target_angle = fmod(target_angle, 2*PI);

	do{

		velocity_publisher.publish(vel_msg);
		current_angle = turtlesim_pose.theta;

		ros::spinOnce();
	}while(fabs(target_angle - current_angle) > 0.01);

	vel_msg.angular.z = 0;
	velocity_publisher.publish(vel_msg);

}
