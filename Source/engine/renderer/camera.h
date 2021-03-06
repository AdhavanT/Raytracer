#pragma once
#include "PL/PL_math.h"
#include "settings.h"
#include "ray.h"
#include "renderer.h"

//Camera always faces along -z camera axis
//this is to consistent with opengl and the "right hand" system 
struct Camera
{
	RenderSettings render_settings;

	f32 aspect_ratio;
	vec3f camera_z, camera_x, camera_y;

	vec3f eye = { 0 }; //position of virtual eye
	vec3f frame_center; //center of camera "sensor"

	f32 h_fov = 1;
	f32 half_pixel_width = 0, half_pixel_height = 0;
};

static inline void update_camera_pos(Camera& cm, vec3f eye, vec3f facing_towards)
{
	cm.eye = eye;
	normalize(facing_towards);
	cm.frame_center = cm.eye + facing_towards;
	cm.camera_z = -facing_towards;
	vec3f y_axis = { 0.f,1.f,0.f };
	cm.camera_x = cross(y_axis,cm.camera_z );
	normalize(cm.camera_x);
	cm.camera_y = cross( cm.camera_z, cm.camera_x);
	normalize(cm.camera_y);
}

static inline void set_camera(Camera& cm, vec3f eye, vec3f facing_towards, RenderSettings &render_settings, f32 h_fov_)
{
	cm.h_fov = h_fov_;
	cm.render_settings = render_settings;
	cm.aspect_ratio = render_settings.resolution.x / (f32)render_settings.resolution.y;
	update_camera_pos(cm, eye, facing_towards);

	cm.half_pixel_width = (0.5f * cm.h_fov) / (f32)render_settings.resolution.x;
	cm.half_pixel_height = 0.5f / (f32)render_settings.resolution.y;
}
