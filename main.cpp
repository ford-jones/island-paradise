#include <lazarus.h> // v0.9.0
#include <iostream>
#include <string>

_Float32 move_light_x   = 0.0f;
_Float32 move_light_z   = 0.0f;
_Float32 move_cam_x     = 0.0f;
_Float32 move_cam_z     = 0.0f;
_Float32 rotate_cam_x   = 0.0f;
_Float32 rotate_cam_y   = 0.0f;

bool quitn_time = false;

void process_event(int32_t key_code)
{
    switch (key_code)
    {
        /*1*/
        case 49:
            move_light_x = 0.5f;
            break;
        /*2*/
        case 50:
            move_light_x = -0.5f;
            break;
        /*3*/
        case 51:
            move_light_z = -0.5f;
            break;
        /*4*/
        case 52: 
            move_light_z = 0.5f;
            break;
        /*Right*/
        case 262:
            move_cam_x = 0.5f;
            break;
        /*Left*/
        case 263:
            move_cam_x = -0.5f;
            break;
        /*Down*/
        case 264:
            move_cam_z = -0.5f;
            break;
        /*Up*/
        case 265:
            move_cam_z = 0.5f;
            break;
        /*W*/
        case 87:
            rotate_cam_x -= 1.0f;
            break;
        /*A*/
        case 65:
            rotate_cam_y -= 1.0f;
            break;
        /*S*/
        case 83:
            rotate_cam_x += 1.0f;
            break;
        /*D*/
        case 68:
            rotate_cam_y += 1.0f;
            break;
        /*Spacebar*/
        case 32:
            quitn_time = true;
            break;
        /*Nothin*/
        case 0:
            move_light_x = 0.0f;
            move_light_z = 0.0f;
            move_cam_x  = 0.0f;
            move_cam_z  = 0.0f;
            break;
        default:
            break;
    }

    if(rotate_cam_x > 360.0f || rotate_cam_x < -360) rotate_cam_x = 0;
};

int main()
{
    Lazarus::GlobalsManager globals = Lazarus::GlobalsManager();
    globals.setLaunchInFullscreen(true);
    globals.setCursorHidden(true);

    Lazarus::WindowManager window("Island");
    Lazarus::Shader shader_program = Lazarus::Shader();
    Lazarus::Transform transformer = Lazarus::Transform();
    
    window.monitorPixelOccupants();

    window.createWindow();
    window.eventsInit();
    globals.setEnforceImageSanity(true);
    globals.setMaxImageSize(500, 500);

    window.loadConfig();

    int32_t default_shader = shader_program.compileShaders();
    shader_program.setActiveShader(default_shader);
    
    Lazarus::LightManager light_manager(default_shader);
    Lazarus::LightManager::Light light = light_manager.createLightSource(0.0f, 30.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1000.0f);
    
    Lazarus::CameraManager camera_manager(default_shader);
    Lazarus::CameraManager::Camera camera = camera_manager.createPerspectiveCam();
    
    transformer.translateCameraAsset(camera, 0.0, 5.0, 0.0);
    transformer.translateLightAsset(light, 0.0f, 2.5f, 0.0f);

    Lazarus::WorldFX world_manager(default_shader);
    Lazarus::WorldFX::SkyBox skybox = world_manager.createSkyBox("images/pos_x.png", "images/neg_x.png", "images/neg_y.png", "images/pos_y.png", "images/pos_z.png", "images/neg_z.png");
    Lazarus::WorldFX::Fog atmosphere = world_manager.createFog(14.0, 80.0, 0.5, 0.2, 0.4, 0.6);
    
    Lazarus::MeshManager terrain_manager(default_shader);
    Lazarus::MeshManager::Mesh terrain = terrain_manager.create3DAsset("mesh/plains.obj", "material/plains.mtl", "images/grass.png");
    
    int32_t caustics_shader = shader_program.compileShaders("shaders/caustics.glsl");
    shader_program.setActiveShader(caustics_shader);

    Lazarus::MeshManager water_manager(caustics_shader);
    Lazarus::MeshManager::Mesh water = water_manager.create3DAsset("mesh/river.obj", "material/river.mtl");

    window.open();
    
    while(window.isOpen)
    {
        
        /*Events*/
        window.monitorEvents();
        window.monitorElapsedUptime();
        process_event(window.keyEventCode);
        
        /*Shaders*/
        shader_program.setActiveShader(default_shader);

        /*Fog*/
        world_manager.drawSkyBox(skybox, camera);
        world_manager.loadFog(atmosphere);
        
        /*Light*/
        light_manager.loadLightSource(light);
        transformer.translateLightAsset(light, move_light_x, 0.0, move_light_z);
        
		/*Camera*/
        transformer.translateCameraAsset(camera, move_cam_x, 0.0f, move_cam_z);
        transformer.rotateCameraAsset(camera, rotate_cam_x, rotate_cam_y, 0.0f);
        camera_manager.loadCamera(camera);
        
        /*Mesh*/
        terrain_manager.loadMesh(terrain);
        terrain_manager.drawMesh(terrain);
        
        shader_program.setActiveShader(caustics_shader);
        shader_program.uploadUniform("time", &window.elapsedTime);

        // world_manager.loadFog(atmosphere);
        light_manager.loadLightSource(light);
        camera_manager.loadCamera(camera);
        water_manager.loadMesh(water);
        water_manager.drawMesh(water);

        window.presentNextFrame();

        uint32_t status = globals.getExecutionState();

        // if(status != LAZARUS_OK)
        // {
        //     std::cout << "Engine status: " << status << std::endl; 
        //     window.close();
        // };
        
        if(quitn_time) window.close();
    };

    return 1;
};