#include <Arduino.h>
#include "pid.h"

vector zero_vect 	= {0.,0.,0.,0.,0.};
vector unit_x 		= {1.,0.,1.,0.,1.};
vector unit_y 		= {0.,1.,0.,1.,1.};

PID_Manager yaw_pid, roll_pid, pitch_pid;
float global_timeout = 0.020; //seconds

int pid_setup() {
	//setup yaw pid
	yaw_pid.timeout 	= global_timeout;
	yaw_pid.k_p			= 10.;
	yaw_pid.k_i			= 0.;
	yaw_pid.k_d			= 0.;
	
	//setup roll pid
	roll_pid.timeout 	= global_timeout;
	roll_pid.k_p		= 10.;
	roll_pid.k_i		= 0.;
	roll_pid.k_d		= 0.;
	
	//setup pitch pid
	pitch_pid.timeout 	= global_timeout;
	pitch_pid.k_p		= 10.;
	pitch_pid.k_i		= 0.;
	pitch_pid.k_d		= 0.;
	
	return 0;
}

int cmd_vehicle(HubsanSession *hubsanSession, PIXY_coordinate copter, vector dest) {
	bool pid_success = true;
	float angle_diff;
	float heading_angle;
	vector yaw_unit;	//unit vector pointing in direction of yaw angle
	vector heading;		//position vector for necessary correction
	vector heading_p;	//projection of correction onto yaw
	vector heading_r;	//vector perpendicular to yaw that is remainder of the correction
	
	//create unit vector in the direction of the yaw angle
	vec_rotate(unit_x, &yaw_unit, copter.yaw); 
	
	//the difference of the vector that points at the center of the copter
	//and the requested destination
	//is the necessary heading of the copter
	vec_diff(copter.center,dest,&heading);
	
	//find the heading angle
	heading_angle = vec_angle(heading,unit_x);
	if (heading.x < 0) heading_angle = 360. - heading_angle;
	
	//calculate difference in angle between heading and yaw in absolute frame of camera
	angle_diff = copter.yaw - heading_angle;
	
	//determine error in direction of yaw vector
	//this is the pitch demand
	vec_proj(heading,yaw_unit,&heading_p);
	//determine error perpendicular to the yaw
	//this is the roll demand
	vec_diff(heading_p,heading,&heading_r);
	
	if (copter.yaw < 180 && copter.yaw > 0) yaw_pid.feedback = copter.yaw;
	else if (copter.yaw <= 360 && copter.yaw >= 180) yaw_pid.feedback = -(360 - copter.yaw);
	else yaw_pid.feedback = 0.;
	
	//RPY PID updates
	//must deal with case where angle flips between 0 and 360 or 180 and -180
	yaw_pid.updated = PID_update(&yaw_pid,90); 	
	pid_success &= yaw_pid.updated;
	
	pitch_pid.feedback = heading_p.len;
	pitch_pid.updated = PID_update(&pitch_pid,0);
	pid_success &= pitch_pid.updated;
	
	roll_pid.feedback = heading_r.len;
	roll_pid.updated = PID_update(&roll_pid,0);
	pid_success &= roll_pid.updated;
	
	//update motor commands
	//conventions may be flipped
	//if not using expert mode range of values is +- 60 (about 50% range)
	//RPY offset by 128 (50% of 255)
	hubsanSession->throttle = 0.5*255; //maybe pid loop this as well and add logic for voltage comp.
	hubsanSession->rudder	= 0.6*yaw_pid.out+128;
	hubsanSession->aileron	= 0.6*roll_pid.out+128;
	hubsanSession->elevator	= 0.6*pitch_pid.out+128;
	
	return pid_success;
};

//subtract vectors
void vec_diff(vector v1, vector v2, vector *v3) {
	v3->x = v2.x - v1.x;
	v3->y = v2.y - v1.y;
	vec_update(v3);
}

//update length and unit vectors
void vec_update(vector *v) {
	v->len = vec_dist(*v,zero_vect);
	v->ux = (v->x)/(v->len);
	v->uy = (v->y)/(v->len);
}

//determine projection of v1 on v2
void vec_proj(vector v1, vector v2, vector *vf) {
	float length;
	length = vec_dot(v1,v2);
	vf->x = length*v2.ux;
	vf->y = length*v2.uy;
	vec_update(vf);
}

//determine distance between points
float vec_dist(vector v1, vector v2) {
	return sqrt((v2.x-v1.x)*(v2.x-v1.x)+(v2.y-v1.y)*(v2.y-v1.y));
}

//Calculates dot product
float vec_dot(vector v1, vector v2) {
	float dot_product;
	dot_product = v1.x*v2.x+v1.y+v2.y;
	return dot_product;
}

//calculates angle between vectors
float vec_angle(vector v1, vector v2) {
	float angle;
	angle = acos(vec_dot(v1,v2)/(v1.len*v2.len));
	return angle;
}

//rotate a vector by theta degrees.
int vec_rotate(vector v, vector *v_r, float theta) {
	//check for a valid angle of rotation
	if (theta >= 0 && theta <= 360) {
		v_r->x = v.x*cos(theta) - v.y*sin(theta);
		v_r->y = v.x*sin(theta) + v.y*cos(theta);
		vec_update(v_r);
		return 1;
	}
	else return 0;
}

//determine yaw angle and center of copter
int get_yaw(PIXY_coordinate *loc) {
	float t1,t2,t3,t4;
	float temp;
	float tmp_yaw[4];
	vector tmp_vec;
	
	//Center is the average of the x and y position of the four LEDs
	loc->center.x = (loc->pos1.x+loc->pos2.x+loc->pos3.x+loc->pos4.x)/4.;
	loc->center.y = (loc->pos1.y+loc->pos2.y+loc->pos3.y+loc->pos4.y)/4.;
	vec_update(&loc->center);
	
	//Define 0 degrees as vector pointing up on image
	//Angle increases counter clockwise
	
	//Use dot product to find the 4 angles of the vectors defined as
	//p1 to p2, p2 to p3, p4 to p3, p1 to p4
	//Note that the 3rd and 4th vector directions are flipped
	//otherwise the sum would be zero.
	//The vectors are dotted with the unit x vector
	//as that is zero degrees.
	vec_diff(loc->pos1,loc->pos2,&tmp_vec);
	tmp_yaw[0] = vec_angle(tmp_vec,unit_x);
	
	vec_diff(loc->pos2,loc->pos3,&tmp_vec);
	tmp_yaw[1] = vec_angle(tmp_vec,unit_x);
	
	vec_diff(loc->pos4,loc->pos3,&tmp_vec);
	tmp_yaw[2] = vec_angle(tmp_vec,unit_x);
	
	vec_diff(loc->pos1,loc->pos4,&tmp_vec);
	tmp_yaw[3] = vec_angle(tmp_vec,unit_x);
	
	//since two of the vectors will necessarily be
	//perpendicular to the other two, subtract 180 degrees from the sum
	temp = tmp_yaw[0]+tmp_yaw[1]+tmp_yaw[2]+tmp_yaw[3] - 180;
	
	//the yaw angle should be the same as the average of the four angles
	loc->yaw = temp/4.;
	
	//can have different return values if the angles have too great of a
	//variance
	
	//also depending on how stable the readings are roll and pitch
	//can be determined from video footage maybe???
	
	return 0;
}

//PID values and configuration need to be setup
//in a separate function
int PID_update(PID_Manager *pid, float demand) {	
	pid->dt = (pid->last_micros - micros())/1000000.;
	
	if (pid->dt > pid->timeout) {
		pid->e_old = 0;
		pid->e_int = 0;
		pid->e_dot = 0;
		pid->out = 0;
		pid->feedback_old = 0;
		pid->last_micros = micros();
		return 0;
	}
	
	pid->e = demand - pid->feedback;	//determine error
	pid->e_int += pid->e*pid->dt;		//update integral error
	pid->e_dot *= 0.8;					//filter and calculate the error derivative
	pid->e_dot += (1.-0.8)*(pid->e - pid->e_old)/pid->dt;
	
	//determine output and limit to +- 100
	//values are to be externally scaled
	pid->out += pid->k_p*pid->e;
	pid->out += pid->k_i*pid->e_int;
	pid->out += pid->k_d*pid->e_dot;
	constrain(pid->out,-100,100);
	
	pid->e_old	= pid->e;
	pid->feedback_old = pid->feedback;
	pid->last_micros = micros();
	
	return 1;
}