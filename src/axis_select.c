#include "../include/program_data.h"
#include "../include/draw_util.h"
#include "../include/raylib.h"
#include "../dynamic_arrays/dynamic_array.h"
#include "../include/raymath.h"

#include <stdio.h>

void AXIS_SELECT_FUNCTION(program_data* data) {

    // MOUSE COLLISION - CHECKING WHICH AXIS IS BEING SELECTED
    Color x_color = BLACK, y_color = BLACK, z_color = BLACK;
    Ray mouse_ray = GetMouseRay(GetMousePosition(), *data->main_camera);
    float distance;

    RayCollision x_collision = GetRayCollisionBox(mouse_ray, (BoundingBox) {V3(-10, -0.5, -0.5), V3(10, 0.5, 0.5)});
    RayCollision y_collision = GetRayCollisionBox(mouse_ray, (BoundingBox) {V3(-0.5, -10, -0.5), V3(0.5, 10, 0.5)});
    RayCollision z_collision = GetRayCollisionBox(mouse_ray, (BoundingBox) {V3(-0.5, -0.5, -10), V3(0.5, 0.5, 10)});

    *data->save_vector = V3(1, 1, 1); // THE VARIABLE THAT WILL BE SENT TO THE NEXT STATE

    if (x_collision.hit) {
        x_color = WHITE;
        data->save_vector->x = 20;
    }

    if (y_collision.hit) {
        x_color = BLACK;
        y_color = WHITE;
        data->save_vector->x = 1;
        data->save_vector->y = 20;
        if (x_collision.hit && x_collision.distance < y_collision.distance) {
            x_color = WHITE;
            y_color = BLACK;
            data->save_vector->x = 20;
            data->save_vector->y = 1;
        }
    }

    if (z_collision.hit) {
        x_color = BLACK;
        y_color = BLACK;
        z_color = WHITE;
        data->save_vector->z = 20;

        if (x_collision.hit && x_collision.distance < z_collision.distance) {
            x_color = WHITE;
            z_color = BLACK;
            data->save_vector->x = 20;
            data->save_vector->z = 1;
        }
        if (y_collision.hit && y_collision.distance < z_collision.distance) {
            y_color = WHITE;
            z_color = BLACK;
            data->save_vector->y = 20;
            data->save_vector->z = 1;
        }
    }
    // END AXIS STUFF


    // HANDLE INPUT
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        DisableCursor();
    }
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
        UpdateCamera(data->main_camera, CAMERA_THIRD_PERSON);
        data->mini_camera->position = Vector3Negate(Vector3Normalize(data->main_camera->position));
        data->mini_camera->up = data->main_camera->up;
    }
    if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
        EnableCursor();
    }
	

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (x_collision.hit || y_collision.hit || z_collision.hit) {
            *data->program_state = GRID_SELECT;
        }
		
		int click_point = MOUSE_POINT_COLLISION(GetMouseRay(GetMousePosition(), *data->main_camera), *(data->points));

        if (click_point == -1) {
            //for (int i=0; i<data->sel_points->current_length; i++) {
            if (data->sel_points->current_length > 0) {
                da_clear(data->sel_points);
            }
        } else {
            printf("clicked point\n");
            size_t click_point_s = click_point;
            da_push(data->sel_points, (void*)&click_point_s);
            printf("sel_points now %d large\n", data->sel_points->current_length);
        }

    }





	DRAW_COMPASS(data->reference_render, data->mini_camera);


    // DRAW MAIN SCREEN
    BeginDrawing();

    	//ClearBackground(STATE_COLORS[*(data->program_state)]);
		ClearBackground(data->colors[*(data->program_state)]);

		DrawRectangle(10, 10, GetScreenWidth() - 20, GetScreenWidth() - 20, GRAY);

        BeginMode3D(*data->main_camera);

            // DRAW MAIN GRID
            draw_axis(V3(0, 0, 0), 10, x_color, y_color, z_color);
			
			DRAW_POINTS(*(data->points), *(data->sel_points));

            DRAW_FACES(*(data->points), *(data->face_points));

        EndMode3D();

        DrawTexture(data->reference_render->texture, 0, GetScreenHeight() - (GetScreenWidth() / 8), WHITE);


    EndDrawing();
}