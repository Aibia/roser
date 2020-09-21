
#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
//#include "rplidar_ros/Control.h"
//#include "rplidar_ros/steer.h"
//#include "rplidar_ros/velocity.h"
#include "std_msgs/Float32.h"
#include "math.h"

#define PI 3.1415
#define INF 10000000
#define RAD2DEG(x) ((x)*180./M_PI)

std_msgs::Float32 pubSteer;
std_msgs::Float32 pubVelocity;
//rplidar_ros::Control pubToArdu;
//ros::Publisher pub;
ros::Publisher pub_steer;
ros::Publisher pub_velocity;


float angle_detected[400] = {0.0,}; 
float distance_detected[400] = {0.0,}; 
float width = 0.0;

float Max_Threshold = 0.6; // 도로 폭 1.2기준
int Max_Steering = 2; //15 degree; 최대 조향 각도설정
float Min_Threshold = 0.0;
int Min_Speed = 1540; // 최대 주행 스피드
bool flag_1 = false;
bool flag_2 = false;
bool go_1 = false;
bool go_2 = false;
bool flag_3 =false;
bool flag_4 = false;
int start = 1;
int set = 0;
float velocity = 0;
/*float degree_to_rad(double degrees){
    return degrees *PI/180;
}*/


float get_distance_left(){ 
    float left_dist = 0;
    float count = 0;
    for(int i =214; i< 217; i++){ //34~36 degree
    	if(distance_detected[i] >= 0 && distance_detected[i]<=10){
	       count ++;
           left_dist += distance_detected[i];
	}
    }
    
    left_dist /= count;
    return left_dist;
}


float get_distance_right(){ 
    float right_dist = 0; 
    float count = 0;
    for(int i =144 ; i< 147; i++){ //-34~-36 degree
	if(distance_detected[i] >=0 && distance_detected[i]<=10){
        count ++;
        right_dist += distance_detected[i];
	}
    }
    right_dist /= count;
    return right_dist;
}



float get_wall_left(){ 
    float l_dist = 0;
    float count = 0;
    for(int i =268; i< 271; i++){ //90~36 degree
    	if(distance_detected[i] >= 0 && distance_detected[i]<=10){
	   count ++;
           l_dist += distance_detected[i];
	}
    }
    
    l_dist /= count;
    return l_dist;
}

float get_wall_right(){ 
    float r_dist = 0; 
    float count = 0;
    for(int i =89 ; i< 90; i++){ //-34~-36 degree
	if(distance_detected[i] >=0 && distance_detected[i]<=10){
        count ++;
        r_dist += distance_detected[i];
	}
    }
    r_dist /= count;
    return r_dist;
}



//범위에 없는 값은 inf로 들어오는데 어떤값인지 확인하고 처리해줄것
/*bool to_close(const sensor_msgs::LaserScan::ConstPtr scan,float distance){
    int offset = degree_to_rad(150) / scan->angle_increment;
    float min = INF;
    int angle = 0;

    for(int i = offset ; i<scan->ranges.size() - offset ; i++){
        if(scan->ranges[i] > 0.01 && scan->ranges[i] < 1.0){
            angle = scan->angle_min + i* scan->angle_increment;
            double dist = laser->range[i]*cos(angle);

            if(dist < distance){
                return true;
            }
        }

    }
    return false;
}*/

float cal_velocity(float steer_angle){

    if(steer_angle <= 110){
        velocity = (18.75)*(steer_angle)-19.625*100+Min_Speed;
        }
    else {
        velocity = -(18.75)*(steer_angle)+21.625*100+Min_Speed;
    }
    if(velocity <= Min_Speed){
        velocity = Min_Speed;
    }    
    

    /*if(steer_angle <= 110){
        velocity = (80/40)*(steer_angle-70) + Min_Speed;
        }
    else {
        velocity = -(80/40)*(steer_angle-150) + Min_Speed;
    }
    if(velocity <= Min_Speed){
        velocity = Min_Speed;
    }*/
    return velocity;
}


void scanCallback(const sensor_msgs::LaserScan::ConstPtr& scan)
{

    int count = scan->scan_time / scan->time_increment; 
    for(int i = 0; i < count; i++) {
        float degree = RAD2DEG(scan->angle_min + scan->angle_increment * i);
        angle_detected[i] = degree;  
        distance_detected[i] = scan->ranges[i];
	//if( degree >= 45 && degree <50 ){
	//ROS_INFO("degree : %f  , distance : %f ", degree , distance_detected[i]);   
	//}
}    


    float ldist = get_distance_left();
    
    float rdist = get_distance_right();

    ROS_INFO("ldist : %f  , rdist : %f ", ldist , rdist);

    float left_wall = get_wall_left();
    float right_wall = get_wall_right();
    if(!(right_wall < 100)){
        right_wall = 10;
    }
    if(!(left_wall < 100)){
        left_wall = 10;
    }
    if(!(rdist < 100)){
        rdist = 10;
    }
     if(!(ldist < 100)){
        ldist = 10;
    }           
    ROS_INFO("left_wall : %f  , right_wall : %f ", left_wall , right_wall);   
    ROS_INFO("Width : %f ", width ); 
    if(!flag_2){ //처음 시작시에만 도로 폭계산
        width = (left_wall + right_wall)/2;
        flag_2 = true;
        if(abs(left_wall - right_wall) > 0.15){
            start = 2;
        }
        else{
            start = 1;
        }

    }

    if(abs(left_wall - right_wall) > 0.25){ // 측면에 쏠려 있는 경우
        go_1 = false;
        go_2 = true;
    }

    else{
        go_1 = true;
        go_2 = false;
    }


    float x = rdist - ldist;
    float steer_angle;



    if(go_1 || (set == 1)){ //직진 주행 알고리즘
        ROS_INFO("Algorithm 1 (straight) ");
        if( x < -Max_Threshold){
            steer_angle = -Max_Steering; 
        }
        else if (-Max_Threshold <= x && x <-Min_Threshold ){
            steer_angle = ((-Max_Steering*pow((x-Min_Threshold),2))/ pow((Min_Threshold-Max_Threshold),2));
            //velocity = cal_velocity(steer_angle);
        }
        else if(-Min_Threshold <= x && x<=Min_Threshold ){
            steer_angle = 0;
            //velocity = cal_velocity(steer_angle);        
        }
        else if(Min_Threshold < x && x <= Max_Threshold){
            steer_angle = ((Max_Steering*pow((x-Min_Threshold),2))/ pow((Min_Threshold-Max_Threshold),2));
            //velocity = cal_velocity(steer_angle);    
        }
        else {
            steer_angle = Max_Steering;
            //velocity = cal_velocity(steer_angle);        
        }
        flag_3 = true; //직선주행시작하면 무조건 직선주행만
    }
    //왼쪽에서 출발하는 경우는 직진까지 끝났을 때는 flag_1이 켜진 상태이며, set이 1로 되어 있다.. 그렇기 때문에 따라서 다시 곡선주행을 하기 위해서는 이걸 다시 바꿔주면 된다.
    //오른쪽에서 출발하는 경우는 직진까지 끝났을 때는 flaog_4가 켜진 상태이며, set이 1로 되어 있기에 무조건 직진만한다.따라서 직선 주행은 제일 마지막이기에 직선 주행이 끝났을 떄
    // 모든 파라메터를 초기화 해준다면,,,? 
    else{
        if(!flag_3){  // 원주행 알고리즘
            ROS_INFO("Algorithm 2 (cornering) ");
            if(left_wall < right_wall){
                if(flag_4 == true){
                    set = 1; //직진 주행알고리즈 ㅁ적용
                }
                if(!flag_4){
                    if( (width- 0.33) >= left_wall  && !(flag_1)) {
                        steer_angle = 85;

                        ROS_INFO("Algorithm 2 (Conering) - right ");
    
                    }
                    else{
                        steer_angle = 135;
                        ROS_INFO("Algorithm 2 (Conering) - after right - left ");
                        flag_1 = true; //여기 한번 들어오면 우측은 해당 x
                                        
                    }
                }
            }
            else{
                if(flag_1 == true){ // 오버슛이 일어나 벗어난 경우
                    set = 1; // 직진주행알고리즘 적용한다.
                }
                if(!flag_1){
                    if( (width-0.337) >= right_wall && !(flag_4)) {
                        steer_angle =135;
                        ROS_INFO("Algorithm 2 (Conering) - left ");
                                                        
                    }
                    else{
                        steer_angle = 85;
                        ROS_INFO("Algorithm 2 (Conering) - after left - right ");
                        flag_4 = true;
                                                                    
                    }
                }
            }
        }
    }
    float steer_angle_real = 0;
    if(go_2){
        steer_angle_real = steer_angle;
    }
    else{
        steer_angle_real = -((steer_angle*8)/3)+110.0;
    }    

    velocity = cal_velocity(steer_angle_real);
    /*if(go_2 && flag_4){
        velocity = 0;
    }*/

    ROS_INFO("velocity : %f  ,steer_angle : %f , ,steer_angle_real : %f ", velocity , steer_angle, steer_angle_real);
    pubSteer.data = steer_angle_real;
    pubVelocity.data = velocity;

    pub_steer.publish(pubSteer);
    pub_velocity.publish(pubVelocity);

}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "rplidar_node_client");
    ros::NodeHandle n;

    ros::Subscriber sub = n.subscribe<sensor_msgs::LaserScan>("/scan", 100, scanCallback);
    //pub = n.advertise<rplidar_ros::Control>("/ToArdu", 1000);	

    pub_steer = n.advertise<std_msgs::Float32>("/ToSteer", 100);	
    pub_velocity = n.advertise<std_msgs::Float32>("/ToVelocity", 100);	
    ros::spin();

    return 0;
}
