#define RAD2DEG 57.296f

#include "session.h"

struct PID_Manager {
	float req;
	float feedback;
	float feedback_old;
	float out;
	float k_p;		//proportional gain
	float k_i;		//integral gain
	float k_d;		//derivative gain
	float e;		//error
	float e_int;	//integral error
	float e_dot;	//derivative error
	float e_old;
	float dt;		//elapsed time since last loop call
	float timeout;
	long last_micros;
	int updated;
};

struct vector {
	float x,y;
	float ux,uy;
	float len;
};

//could just be an array for the vectors I guess
//LEDs
//orientation is 0 degree when video feed shows the order below
//the copter is upside down if the order is mirrored
//  1	2
//	4	3
struct PIXY_coordinate {
	vector pos1;
	vector pos2;
	vector pos3;
	vector pos4;
	vector center;
	float yaw;
};

int pid_setup();
int cmd_vehicle(HubsanSession *hubsanSession, PIXY_coordinate copter, vector dest);
void vec_diff(vector v1, vector v2, vector *v3);
void vec_update(vector *v);
void vec_proj(vector v1, vector v2, vector *vf);
float vec_dist(vector v1, vector v2);
float vec_dot(vector v1, vector v2);
float vec_angle(vector v1, vector v2);
int vec_rotate(vector v, vector *v_r, float theta);
int get_yaw(PIXY_coordinate loc, float yaw);
int PID_update(PID_Manager *pid, float demand);