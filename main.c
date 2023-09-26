#include "raylib.h"
#include "raymath.h"
#include <string.h>

// convenient functions for making vectors/colors
#define V3(x, y, z) ((Vector3) {(float) x, (float) y, (float) z})
#define C4(r, g, b, a) ((Color){r, g, b, a})

Vector3 absolutify(Vector3 v) {
	Vector3 buffer;
	buffer.x = v.x;
	buffer.y = v.y;
	buffer.z = v.z;
	if (v.x < 0) {
		buffer.x *= -1;
	}
	if (v.y < 0) {
		buffer.y *= -1;
	}
	if (v.z < 0) {
		buffer.z *= -1;
	}
	return buffer;
}
	

// draws grid
void draw_grid(Vector3 center, int lines_no, float unit_size, Vector3 up, Color grid_color) {
	/*
    for (int i = -(lines_no/2); i<= lines_no/2; i++) {
        DrawLine3D(Vector3RotateByAxisAngle(V3(center.x + (i*unit_size), center.y, center.z-((lines_no/2) * unit_size)), V3(1, 0, 0), PI / 2),
        Vector3RotateByAxisAngle(V3(center.x + (i*unit_size), center.y, center.z+((lines_no/2) * unit_size)), V3(1, 0, 0), PI / 2),
        grid_color);
        
        DrawLine3D(Vector3RotateByAxisAngle(V3(center.x-((lines_no/2) * unit_size), center.y, center.z + (i*unit_size)), V3(1, 0, 0), PI / 2),
        Vector3RotateByAxisAngle(V3(center.x+((lines_no/2) * unit_size), center.y, center.z + (i*unit_size)), V3(1, 0, 0), PI / 2),
        grid_color);
    }
	*/
	
	for (int i= -(lines_no/2); i<= lines_no/2; i++) {
		//DrawLine3D(Vector3RotateByAxisAngle( V3(center.x - lines_no/2, center.y + 0, center.z + i), up, 90) , Vector3RotateByAxisAngle(V3(center.x + lines_no/2, center.y + 0, center.z + i), up, 90), grid_color);
		//DrawLine3D(V3(center.x + i, center.y + 0, center.z - lines_no/2), V3(center.x + i, center.y + 0, center.z + lines_no/2), grid_color);
		
		DrawLine3D(V3(center.x + (i * up.x), center.y - ((lines_no/2) * up.y), center.z - ((lines_no/2) * up.z)), V3(center.x + (i * up.x), center.y + ((lines_no/2) * up.y), center.z + ((lines_no/2) * up.z)), grid_color);
		DrawLine3D(V3(center.x - ((lines_no/2) * up.x), center.y + (i * up.y), center.z - ((lines_no/2) * up.z)), V3(center.x + ((lines_no/2) * up.x), center.y + (i * up.y), center.z + ((lines_no/2) * up.z)), grid_color);
		DrawLine3D(V3(center.x - ((lines_no/2) * up.x), center.y - ((lines_no/2) * up.y), center.z + (i * up.z)), V3(center.x + ((lines_no/2) * up.x), center.y + ((lines_no/2) * up.y), center.z + (i * up.z)), grid_color);
	}
	
	
}

void draw_axis(Vector3 center, int unit_no, Color x, Color y, Color z) {
    DrawLine3D(Vector3Subtract(center, V3(unit_no, 0, 0)), Vector3Add(center, V3(unit_no, 0, 0)), x);
    DrawLine3D(Vector3Subtract(center, V3(0, unit_no, 0)), Vector3Add(center, V3(0, unit_no, 0)), y);
    DrawLine3D(Vector3Subtract(center, V3(0, 0, unit_no)), Vector3Add(center, V3(0, 0, unit_no)), z);
}

// draws y axis
void draw_y_axis(Vector3 center, int lines_no, float unit_size) {
    DrawLine3D(V3(center.x, center.y - ((lines_no/2) * unit_size), center.z), V3(center.x, center.y + ((lines_no/2) * unit_size), center.z), BLACK);
    for (int i= -(lines_no/2); i<=lines_no/2; i++) {
        DrawLine3D(V3(center.x - (unit_size / 2), center.y + (i * unit_size), center.z), V3(center.x + (unit_size / 2), center.y + (i * unit_size), center.z), BLACK);
        DrawLine3D(V3(center.x, center.y + (i * unit_size), center.z - (unit_size / 2)), V3(center.x, center.y + (i * unit_size), center.z + (unit_size / 2)), BLACK);
        //for (int j = 1; j<=9; j++) {
            //DrawLine3D(V3(center.x - (unit_size / 10), center.y + (i * unit_size + j * (unit_size / 10)), center.z), V3(center.x + (unit_size / 10), center.y + (i * unit_size + j * (unit_size / 10)), center.z), BLACK);
            //DrawLine3D(V3(center.x, center.y + (i * unit_size + j * (unit_size / 10)), center.z - (unit_size / 10)), V3(center.x, center.y + (i * unit_size + j * (unit_size / 10)), center.z + (unit_size / 10)), BLACK);
            //DrawSphere(V3(0, center.y + (i * unit_size + j * (unit_size / 10)), 0), 0.01, BLACK);
        //}
    }
}

typedef enum {
    VIEW_MODE = 0,
    POINT_MODE_SELECT,
    POINT_MODE_PLACE
} STATE;

const int STATE_COUNT = 3;


typedef struct {
    const int* window_width;
    const int* window_height;
    Camera* main_camera;
    Camera* mini_camera;
    RenderTexture2D* reference_render;
    STATE* program_state;
    Vector3* all_points;
    Vector3* save_vector;
} program_data;

//void VIEW_FUNCTION(Camera* camera, Camera *mini_camera, RenderTexture2D* corner_render, const int* screenWidth, const int* screenHeight, STATE* mode) {
void VIEW_FUNCTION(program_data data) {

    // MOUSE COLLISION
    Color x_color = BLACK, y_color = BLACK, z_color = BLACK;
    Ray mouse_ray = GetMouseRay(GetMousePosition(), *data.main_camera);
    float distance;

    RayCollision x_collision = GetRayCollisionBox(mouse_ray, (BoundingBox) {V3(-10, -0.5, -0.5), V3(10, 0.5, 0.5)});
    RayCollision y_collision = GetRayCollisionBox(mouse_ray, (BoundingBox) {V3(-0.5, -10, -0.5), V3(0.5, 10, 0.5)});
    RayCollision z_collision = GetRayCollisionBox(mouse_ray, (BoundingBox) {V3(-0.5, -0.5, -10), V3(0.5, 0.5, 10)});

    *data.save_vector = V3(1, 1, 1); // THE VARIABLE THAT WILL BE SENT TO THE NEXT STATE

    if (x_collision.hit) {
        x_color = WHITE;
        data.save_vector->x = 20;
    }

    if (y_collision.hit) {
        x_color = BLACK;
        y_color = WHITE;
        data.save_vector->x = 1;
        data.save_vector->y = 20;
        if (x_collision.hit && x_collision.distance < y_collision.distance) {
            x_color = WHITE;
            y_color = BLACK;
            data.save_vector->x = 20;
            data.save_vector->y = 1;
        }
    }

    if (z_collision.hit) {
        x_color = BLACK;
        y_color = BLACK;
        z_color = WHITE;
        data.save_vector->z = 20;

        if (x_collision.hit && x_collision.distance < z_collision.distance) {
            x_color = WHITE;
            z_color = BLACK;
            data.save_vector->x = 20;
            data.save_vector->z = 1;
        }
        if (y_collision.hit && y_collision.distance < z_collision.distance) {
            y_color = WHITE;
            z_color = BLACK;
            data.save_vector->y = 20;
            data.save_vector->z = 1;
        }
    }


    // HANDLE INPUT
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        DisableCursor();
    }
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
        UpdateCamera(data.main_camera, CAMERA_THIRD_PERSON);
        data.mini_camera->position = Vector3Negate(Vector3Normalize(data.main_camera->position));
        data.mini_camera->up = data.main_camera->up;
    }
    if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
        EnableCursor();
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (x_collision.hit || y_collision.hit || z_collision.hit) {
            if (*data.program_state == VIEW_MODE) {
                *data.program_state = POINT_MODE_SELECT;
            }
        }
    }

    // DRAW XYZ AXIS ONTO SEPERATE RENDER INSTANCE
    BeginTextureMode(*data.reference_render);
        BeginMode3D(*data.mini_camera);
            ClearBackground((Color){100, 100, 100, 255});
            DrawLine3D(V3(0, 0, 0), V3(-0.25, 0, 0), C4(255, 0, 0, 255));
            DrawLine3D(V3(0, 0, 0), V3(0.25, 0, 0), C4(0, 255, 255, 255));
            DrawCylinderEx(V3(-0.15, 0, 0), V3(-0.1, 0, 0), 0, 0.025, 10, C4(255, 0, 0, 255));
            DrawCylinderEx(V3(0.15, 0, 0), V3(0.1, 0, 0), 0.025, 0, 10, C4(0, 255, 255, 255));
            //DrawCylinderEx(V3(0, 0, 0), V3(-0.25, 0, 0), 0.015, 0.015, 10, C4(255, 0, 0, 255));
            //DrawCylinderEx(V3(0, 0, 0), V3(0.25, 0, 0), 0.01, 0.01, 10, C4(255, 0, 0, 127));

            DrawLine3D(V3(0, 0, 0), V3(0, -0.25, 0), C4(0, 255, 0, 255));
            DrawLine3D(V3(0, 0, 0), V3(0, 0.25, 0), C4(255, 0, 255, 255));
            DrawCylinderEx(V3(0, -0.15, 0), V3(0, -0.1, 0), 0, 0.025, 10, C4(0, 255, 0, 255));
            DrawCylinderEx(V3(0, 0.15, 0), V3(0, 0.1, 0), 0.025, 0, 10, C4(255, 0, 255, 255));
            //DrawCylinderEx(V3(0, 0, 0), V3(0, -0.25, 0), 0.015, 0.015, 10, C4(0, 255, 0, 255));
            //DrawCylinderEx(V3(0, 0, 0), V3(0, 0.25, 0), 0.01, 0.01, 10, C4(255, 0, 255, 255));

            DrawLine3D(V3(0, 0, 0), V3(0, 0, -0.25), C4(0, 0, 255, 255));
            DrawLine3D(V3(0, 0, 0), V3(0, 0, 0.25), C4(255, 255, 0, 255));
            DrawCylinderEx(V3(0, 0, -0.15), V3(0, 0, -0.1), 0, 0.025, 10, C4(0, 0, 255, 255));
            DrawCylinderEx(V3(0, 0, 0.15), V3(0, 0, 0.1), 0.025, 0, 10, C4(255, 255, 0, 255));
            //DrawCylinderEx(V3(0, 0, 0), V3(0, 0, -0.25), 0.015, 0.015, 10, C4(0, 0, 255, 255));
            //DrawCylinderEx(V3(0, 0, 0), V3(0, 0, 0.25), 0.01, 0.01, 10, C4(255, 255, 0, 255));

        EndMode3D();
    EndTextureMode();




    // DRAW MAIN SCREEN
    BeginDrawing();

        ClearBackground(GRAY);

        BeginMode3D(*data.main_camera);

            // DRAW MAIN GRID
            draw_axis(V3(0, 0, 0), 10, x_color, y_color, z_color);

            DrawSphere(data.all_points[0], 0.5, BLACK);

        EndMode3D();

        DrawTexture(data.reference_render->texture, 0, *data.window_height - (*data.window_width / 8), WHITE);

    EndDrawing();
    

}

//void POINT_SELECT_FUNCTION(Camera* camera, Camera *mini_camera, RenderTexture2D* corner_render, const int* screenWidth, const int* screenHeight, STATE* mode) {
void POINT_SELECT_FUNCTION(program_data data) {

    // MOUSE COLLISION
    //Color x_color = BLACK, y_color = BLACK, z_color = BLACK;
    Ray mouse_ray = GetMouseRay(GetMousePosition(), *data.main_camera);
    //float distance;

    //RayCollision x_collision = GetRayCollisionBox(mouse_ray, (BoundingBox) {V3(-10, -0.5, -0.5), V3(10, 0.5, 0.5)});
    //RayCollision y_collision = GetRayCollisionBox(mouse_ray, (BoundingBox) {V3(-0.5, -10, -0.5), V3(0.5, 10, 0.5)});
    RayCollision axis_collision = GetRayCollisionBox(mouse_ray, (BoundingBox) {Vector3Multiply(V3(-0.5, -0.5, -0.5), *data.save_vector), Vector3Multiply(V3(0.5, 0.5, 0.5), *data.save_vector)});

    

    // HANDLE INPUT
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        DisableCursor();
    }
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
        UpdateCamera(data.main_camera, CAMERA_THIRD_PERSON);
        data.mini_camera->position = Vector3Negate(Vector3Normalize(data.main_camera->position));
        data.mini_camera->up = data.main_camera->up;
    }
    if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
        EnableCursor();
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        data.all_points[0] = V3((int) (axis_collision.point.x), (int) (axis_collision.point.y), (int) (axis_collision.point.z));
        *data.program_state = VIEW_MODE;
    }


    // DRAW XYZ AXIS ONTO SEPERATE RENDER INSTANCE
    BeginTextureMode(*data.reference_render);
        BeginMode3D(*data.mini_camera);

            ClearBackground((Color){100, 100, 100, 255});

            DrawLine3D(V3(0, 0, 0), V3(-0.25, 0, 0), C4(255, 0, 0, 255));
            DrawLine3D(V3(0, 0, 0), V3(0.25, 0, 0), C4(0, 255, 255, 255));
            DrawCylinderEx(V3(-0.15, 0, 0), V3(-0.1, 0, 0), 0, 0.025, 10, C4(255, 0, 0, 255));
            DrawCylinderEx(V3(0.15, 0, 0), V3(0.1, 0, 0), 0.025, 0, 10, C4(0, 255, 255, 255));

            DrawLine3D(V3(0, 0, 0), V3(0, -0.25, 0), C4(0, 255, 0, 255));
            DrawLine3D(V3(0, 0, 0), V3(0, 0.25, 0), C4(255, 0, 255, 255));
            DrawCylinderEx(V3(0, -0.15, 0), V3(0, -0.1, 0), 0, 0.025, 10, C4(0, 255, 0, 255));
            DrawCylinderEx(V3(0, 0.15, 0), V3(0, 0.1, 0), 0.025, 0, 10, C4(255, 0, 255, 255));

            DrawLine3D(V3(0, 0, 0), V3(0, 0, -0.25), C4(0, 0, 255, 255));
            DrawLine3D(V3(0, 0, 0), V3(0, 0, 0.25), C4(255, 255, 0, 255));
            DrawCylinderEx(V3(0, 0, -0.15), V3(0, 0, -0.1), 0, 0.025, 10, C4(0, 0, 255, 255));
            DrawCylinderEx(V3(0, 0, 0.15), V3(0, 0, 0.1), 0.025, 0, 10, C4(255, 255, 0, 255));

        EndMode3D();
    EndTextureMode();



	Vector3 up = V3((data.save_vector->x == 1) ? 1 : 0, (data.save_vector->y == 1) ? 1 : 0, (data.save_vector->z == 1) ? 1 : 0);

    // DRAW MAIN SCREEN
    BeginDrawing();

        ClearBackground(GRAY);

        BeginMode3D(*data.main_camera);

            // DRAW MAIN GRID
            draw_axis(V3(0, 0, 0), 10, BLACK, BLACK, BLACK);
            if (axis_collision.hit) {
                //DrawSphere(axis_collision.point, 0.5, BLACK);
                DrawSphere(V3((int) (axis_collision.point.x), (int) (axis_collision.point.y), (int) (axis_collision.point.z)), 0.25, BLACK);
                //draw_grid(V3((int) (axis_collision.point.x), (int) (axis_collision.point.y), (int) (axis_collision.point.z)), 5, 1, Vector3Subtract(V3(1, 1, 1), absolutify(Vector3Normalize(V3((int) (axis_collision.point.x), (int) (axis_collision.point.y), (int) (axis_collision.point.z))))), BLACK);
                draw_grid(V3((int) (axis_collision.point.x), (int) (axis_collision.point.y), (int) (axis_collision.point.z)), 5, 1, up, BLACK);

            }

        EndMode3D();

        DrawTexture(data.reference_render->texture, 0, *data.window_height - (*data.window_width / 8), WHITE);

    EndDrawing();
    

}

// add a union parameter that lets you add input to the next state so that
// it stores minimal data that's interpreted depending on the state
//typedef void (*STATE_FUNCTION) (Camera*, Camera*, RenderTexture2D*, const int*, const int*, STATE*);
typedef void (*STATE_FUNCTION) (program_data);



int main(void)
{

    // INITIALIZE WINDOW
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - core world screen");

    // INITIALIZE CAMERA
    Camera camera = { 0 };
    camera.position = (Vector3){ 10.0f, 10.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Camera mini_camera = camera;
    mini_camera.position = Vector3Negate(Vector3Normalize(camera.position));

    // INITIALIZE PROGRAM STATE
    STATE mode = VIEW_MODE;
    STATE_FUNCTION mode_functions[3] = {&VIEW_FUNCTION, &POINT_SELECT_FUNCTION, &VIEW_FUNCTION};

    // INITIALIZE RENDERING NONSENSE
    SetTargetFPS(60);

    RenderTexture2D corner_render = LoadRenderTexture(screenWidth / 8, screenWidth / 8);

    //void (*view_fun) (Camera*, Camera*, RenderTexture2D*, const int*, const int*, STATE*);

    STATE_FUNCTION view_fun;

    view_fun = &VIEW_FUNCTION;

    Vector3 all_points[20];
    memset(all_points, 0, 20);

    Vector3 save_vector = V3(0, 0, 0);

    // MAIN LOOP
    while (!WindowShouldClose())
    {

        //view_fun(&camera, &mini_camera, &corner_render, &screenWidth, &screenHeight, &mode);
        //mode_functions[mode](&camera, &mini_camera, &corner_render, &screenWidth, &screenHeight, &mode);
        mode_functions[mode]((program_data) {&screenWidth, &screenHeight, &camera, &mini_camera, &corner_render, &mode, all_points, &save_vector});

    }

    CloseWindow();

    return 0;
}