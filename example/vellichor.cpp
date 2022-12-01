#include "vellichor.h"

extern Window g_window;
extern InputManager g_keyboard;

void createWorld()
{
    Mesh& square = mesh::get("square");
    Mesh& cube = mesh::get("cube");

    Material objectDefault = Material {shader::get("obj_shader"), 1, 1, 0.5f, color::WHITE, 32.0f};
    Material objectDull = Material {shader::get("obj_shader"), 1.0f, 1.0f, 0.1f, color::WHITE, 8.0f, false};

    Mesh shelfMesh = loadObjFile("shelf.obj");
    Mesh winMesh = loadObjFile("window.obj");
    Mesh ladderMesh = loadObjFile("ladder.obj");
    Mesh wallMesh = loadObjFile("wall.obj");

    float start = 30.0f;

    Object shelfArray("shelfArray"), roomHalf("roomHalf");
    Object windowLight("windowLight");
    windowLight.addComponent<Transform>(Transform(Vector3(-10.0f, 3.0f, 1.75f+start), 1));
    windowLight.addComponent<SpotLight>
    ({
        Vector3(1, -0.25f, 0),
        color::WHITE,
        5.0f,
        object::brightness(10),
        std::cos(math::radians(45)), std::cos(math::radians(40))
    });
    shelfArray.addChild(windowLight);

    Object windowLight2 = windowLight.clone();
    windowLight2.getComponent<Transform>() = Transform(Vector3(-10.0f, 3.0f, -1.75f+start), 1);
    windowLight2.getComponent<SpotLight>().direction = Vector3(1, -0.25f, 0);
    shelfArray.addChild(windowLight2);

    Object shelf("shelf");
    shelf.addComponent<Transform>(Transform(Vector3(5, -16.5f, start), 1));
    shelf.addComponent<Model>({color::WHITE, objectDefault, shelfMesh, texture::get("Vellichor/bookcase_texture.png")});
    shelfArray.addChild(shelf);

    Object win("window");
    win.addComponent<Transform>(Transform(Vector3(-9.0f, -2.5f, start), 1));
    win.addComponent<Model>({Color(0.4f, 0.4f, 0.4f, 1.0f), objectDull, winMesh, texture::get("Vellichor/window_texture.png")});
    shelfArray.addChild(win);

    Object back("back");
    back.addComponent<Transform>(Transform(Vector3(-20.0f, 5, 0), Vector3(180, 60, 0), Quaternion(math::radians(90), vec3::up)));
    back.addComponent<Model>({Color(0.02f, 0.02f, 0.05f, 1), objectDefault, square, texture::get("default.png")});

    Object back2("back");
    back2.addComponent<Transform>(Transform(Vector3(120.0f, 5, 0), Vector3(180, 60, 0), Quaternion(math::radians(90), vec3::down)));
    back2.addComponent<Model>({Color(0.02f, 0.02f, 0.05f, 1), objectDefault, square, texture::get("default.png")});

    Object pane("windowPane");
    pane.addComponent<Transform>(Transform(Vector3(-9.45f, 5, start), 7.25f, Quaternion(math::radians(90), vec3::up)));
    pane.addComponent<Model>({color::CLEAR, objectDefault, square, texture::get("default.png")});
    shelfArray.addChild(pane);

    Object ladder("ladder");
    ladder.addComponent<Transform>(Transform(Vector3(-4.9125f, -16.5f, 5.25f+start), 1, Quaternion(math::radians(45), vec3::up)));
    ladder.addComponent<Model>({color::WHITE, objectDefault, ladderMesh, texture::get("Vellichor/ladder_texture.png")});
    ladder.addComponent<BoxCollider>();
    shelfArray.addChild(ladder);

    Object ladder2 = ladder.clone();
    ladder.getComponent<Transform>() = Transform(Vector3(14.9125f, -16.5f, 5.25f+start), 1, Quaternion(math::radians(45), vec3::down));
    shelfArray.addChild(ladder2);

    roomHalf.addChild(shelfArray);

    for(int i=0; i<3; i++)
    {
        Object shelfArray2 = shelfArray.clone();
        for(Object child : shelfArray2.children())
        {
            Transform& childTransform = child.getComponent<Transform>();
            childTransform.position = childTransform.position + Vector3(0, 0, -25*(i+1));
        }
        roomHalf.addChild(shelfArray2);
    }

    Object roomHalf2 = roomHalf.clone();
    for(Object array : roomHalf2.children())
    {
        for(Object child : array.children())
        {
            Transform& childTransform = child.getComponent<Transform>();
            childTransform.position += Vector3(91, 0, 0);
            if(child.name.find("window") != std::string::npos)
            {
                childTransform.position += Vector3(28.0f, 0, 0);
                if(child.name.find("Light") != std::string::npos)
                {
                    child.getComponent<SpotLight>().direction *= Vector3(-1, 1, -1);
                }
                else
                {
                    childTransform.rotation *= Quaternion(math::radians(180), vec3::up);
                }
            }
        }
    }

    Object wall("wall");
    wall.addComponent<Transform>(Transform(Vector3(50.5f, -5, -60), 1));
    wall.addComponent<Model>({color::WHITE, objectDull, wallMesh, texture::get("Vellichor/wall_texture.png")});

    Object wall2 = wall.clone();
    wall2.getComponent<Transform>() = Transform(Vector3(-9.5f, -5, 0), 1 , Quaternion(math::radians(90), vec3::up));

    Object wall3 = wall.clone();
    wall3.getComponent<Transform>() = Transform(Vector3(110.5f, -5, 0), 1 , Quaternion(math::radians(90), vec3::down));
    wall3.getComponent<Model>().material = Material {shader::get("obj_shader"), 0.5f, 0.0f, 0.0f, color::WHITE, 64.0f};

    Object wall4 = wall.clone();
    wall4.getComponent<Transform>() = Transform(Vector3(50.5f, -5, 60), 1 , Quaternion(math::radians(180), vec3::up));

    Object floor("floor");
    floor.addComponent<Transform>(Transform(Vector3(50.5f, -32.5f, 0), 120, Quaternion(math::radians(90), vec3::left)));
    floor.addComponent<Model>({Color(0.2f, 0.02f, 0.03f, 1), Material {shader::get("obj_shader"), 1, 0, 0, color::WHITE, 64.0f}, shape::square(24), texture::get("Vellichor/carpet_texture.png")});

    Object ceiling("ceiling");
    ceiling.addComponent<Transform>(Transform(Vector3(50.5f, 19.5f, 0), 120, Quaternion(math::radians(90), vec3::right)));
    ceiling.addComponent<Model>({Color(0.025f, 0.01f, 0.01f, 1), Material {shader::get("obj_shader"), 1, 0, 0, color::WHITE, 64.0f}, shape::square(24), texture::get("default.png")});
}

void register8DStates(AnimationState& state)
{
    state.addParameter<Vector3>("force", vec3::zero);
    auto inRange = [](const Vector3& one, const Vector3& two)
    {
        return (one-two).length() < std::sqrt(2-std::sqrt(2))/2;
    };
    
    state.addTransition<Vector3>("force", inRange, vec3::back, "north");
    state.addTransition<Vector3>("force", inRange, (vec3::back + vec3::left).normalized(), "nwest");
    state.addTransition<Vector3>("force", inRange, vec3::left, "west");
    state.addTransition<Vector3>("force", inRange, (vec3::forward + vec3::left).normalized(), "swest");
    state.addTransition<Vector3>("force", inRange, vec3::forward, "south");
    state.addTransition<Vector3>("force", inRange, (vec3::forward + vec3::right).normalized(), "seast");
    state.addTransition<Vector3>("force", inRange, vec3::right, "east");
    state.addTransition<Vector3>("force", inRange, (vec3::back + vec3::right).normalized(), "neast");
}

Animator& configurePlayerAnimator(Object& violet)
{
    Animator& animator = violet.addComponent<Animator>();
    animator.onChange(machine::recursiveChange);
    animator.addParameter<bool>("moving", false);

    AnimationState& idle = animator.add(AnimationState(), "idle");
    idle.onUpdate(anim::reset);
    idle.onChange(anim::keep);

    AnimationState& move = animator.add(AnimationState(), "move");
    move.onUpdate(anim::reset);
    move.onChange(anim::keep);


    int idleFrameCount = 7;
    idle.add(Animation(texture::get("Vellichor/Violet/Idle/violet_idle", {"0000", "0001", "0002"}, "png"), idleFrameCount), "south");
    idle.add(Animation(texture::get("Vellichor/Violet/Idle/violet_idle", {"0003", "0004", "0005"}, "png"), idleFrameCount), "swest");
    idle.add(Animation(texture::get("Vellichor/Violet/Idle/violet_idle", {"0006", "0007", "0008"}, "png"), idleFrameCount), "west");
    idle.add(Animation(texture::get("Vellichor/Violet/Idle/violet_idle", {"0009", "0010", "0011"}, "png"), idleFrameCount), "nwest");
    idle.add(Animation(texture::get("Vellichor/Violet/Idle/violet_idle", {"0012", "0013", "0014"}, "png"), idleFrameCount), "north");
    idle.add(Animation(texture::get("Vellichor/Violet/Idle/violet_idle", {"0015", "0016", "0017"}, "png"), idleFrameCount), "neast");
    idle.add(Animation(texture::get("Vellichor/Violet/Idle/violet_idle", {"0018", "0019", "0020"}, "png"), idleFrameCount), "east");
    idle.add(Animation(texture::get("Vellichor/Violet/Idle/violet_idle", {"0021", "0022", "0023"}, "png"), idleFrameCount), "seast");
    register8DStates(idle);

    int moveFrameCount = 5;
    move.add(Animation(texture::get("Vellichor/Violet/Move/violet_move00", {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09"}, "png"), moveFrameCount), "south");
    move.add(Animation(texture::get("Vellichor/Violet/Move/violet_move00", {"10", "11", "12", "13", "14", "15", "16", "17", "18", "19"}, "png"), moveFrameCount), "swest");
    move.add(Animation(texture::get("Vellichor/Violet/Move/violet_move00", {"20"}, "png"), moveFrameCount), "west");
    move.add(Animation(texture::get("Vellichor/Violet/Move/violet_move00", {"21"}, "png"), moveFrameCount), "nwest");
    move.add(Animation(texture::get("Vellichor/Violet/Move/violet_move00", {"22"}, "png"), moveFrameCount), "north");
    move.add(Animation(texture::get("Vellichor/Violet/Move/violet_move00", {"23"}, "png"), moveFrameCount), "neast");
    move.add(Animation(texture::get("Vellichor/Violet/Move/violet_move00", {"24"}, "png"), moveFrameCount), "east");
    move.add(Animation(texture::get("Vellichor/Violet/Move/violet_move00", {"25"}, "png"), moveFrameCount), "seast");
    register8DStates(move);

    animator.addTransition<bool>("moving", Operation::equals, true, std::pair{"idle", "move"});
    animator.addTransition<bool>("moving", Operation::equals, false, std::pair{"move", "idle"});

    return animator;
}


void vellichor::initialize()
{
    struct Event : Script
    {
        Animator *animator;
        Camera *camera;
        Transform *violetTransform, *cameraTransform, *spotTransform;
        
        Vector2 lastMousePosition;
        Vector3 cameraPosition, bufferDirection = vec3::forward, lastCamera = vec3::forward, direction = vec3::forward, force, forceDirection;

        float speed = 2.0f, rotationalSpeed = 20.0f, radius = 6, angle = 0, mouseSensitivity = 0.5f;
        Vector3 circularMotion = Vector3(radius * std::sin(angle), 0, radius * std::cos(angle));
    };
    Event& event = object::initializeScript<Event>();
    {
        // LOAD
        event.load([]
        (System& script)
        {
            texture::set("Vellichor/", {"wall_texture", "bookcase_texture", "ladder_texture", "carpet_texture", "window_texture"}, "png", GL_SRGB_ALPHA);
            texture::set("Vellichor/Violet/Idle/violet_idle00", {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"}, "png", GL_SRGB_ALPHA);
            texture::set("Vellichor/Violet/Move/violet_move00", {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25"}, "png", GL_SRGB_ALPHA);
            texture::set("Vellichor/Mother/Idle/mother_idle00", {"00"}, "png", GL_SRGB_ALPHA);

            g_window.enableVSync(true);
            g_window.lockMouse(true);

            Event& data = script.data<Event>();

            data.lastMousePosition = g_window.mouseScreenPosition();
            createWorld();

            Material uiMaterial =    Material {shader::get("ui_shader")};
            Material playerMat =     Material {shader::get("obj_shader"), 1.0f, 0.2f, 0.1f, color::WHITE, 64.0f};
            Material objectDefault = Material {shader::get("obj_shader"), 1, 1, 0.5f, color::WHITE, 32.0f};

            // Object mother("mother");
            // mother.addComponent<Transform>(Transform(Vector3(0, 2.5f, 15), Vector3(5.0f, 5.0f, 0), Quaternion(math::radians(90), vec3::up)));
            // mother.addComponent<Model>({color::WHITE, playerMat, mesh::get("square"), texture::get("Vellichor/Mother/Idle/mother_idle0000.png")});

            Object violet("violet");
            violet.addComponent<Model>({color::WHITE, playerMat, mesh::get("square"), texture::get("Vellichor/Violet/Idle/violet_idle0000.png")});
            data.violetTransform = &violet.addComponent<Transform>(Transform(Vector3(0, 0.6f, 5), Vector3(1.125f, 2.25f, 1)));
            violet.addComponent<BoxCollider>(BoxCollider(true)).setTrigger([]
            (Entity entity, Entity compare, int triggered)
            {
                float climbSpeed = 2, maxHeight = 0.6f, minHeight = -31.5f;
                if(g_keyboard.inputs[GLFW_KEY_SPACE].held)
                {
                    Transform &transform = object::getComponent<Transform>(entity);
                    Vector3 newTransform = transform.position + Vector3(0, climbSpeed * g_time.deltaTime, 0);
                    transform.position = Vector3(newTransform.x, math::clamp(newTransform.y, minHeight, maxHeight), newTransform.z);
                }
                if(g_keyboard.inputs[GLFW_KEY_LEFT_SHIFT].held)
                {
                    Transform &transform = object::getComponent<Transform>(entity);
                    Vector3 newTransform = transform.position - Vector3(0, climbSpeed * g_time.deltaTime, 0);
                    transform.position = Vector3(newTransform.x, math::clamp(newTransform.y, minHeight, maxHeight), newTransform.z);
                }
            });
            data.animator = &configurePlayerAnimator(violet);

            Object spotLight("follower");
            data.spotTransform = &spotLight.addComponent<Transform>(Transform(Vector3(object::find("violet").getComponent<Transform>().position + Vector3(0, 3, 0)), 0));
            spotLight.addComponent<Model>({color::CLEAR, objectDefault, mesh::get("cube")});
            spotLight.addComponent<SpotLight>({vec3::down, color::WHITE, 4.0f, object::brightness(2), std::cos(math::radians(25.0f)), std::cos(math::radians(20.0f))});

            Object camera("camera");
            data.camera = &camera.addComponent<Camera>(Camera(2.0f, Color(0.01f, 0, 0.005f, 1), Vector3(0, -0.15, -1), vec3::up));
            data.cameraTransform = &camera.addComponent<Transform>(Transform{Vector3(data.violetTransform -> position + Vector3(0, 1.25f, 6))});
            g_window.screen.camera = camera.data;

            violet.addComponent<Billboard>(Billboard{camera.data});
            // mother.addComponent<Billboard>(Billboard{camera.data});
        });

        // UPDATE
        event.update([]
        (System& script)
        {
            Event& data = script.data<Event>();

            data.violetTransform -> position += data.forceDirection * data.speed * g_time.deltaTime;
            data.spotTransform -> position = data.violetTransform -> position + Vector3(0, 3, 0);

            Vector3 playerOffset = data.violetTransform -> position + Vector3(0, 1.25f, 0);
            Vector3 target = Vector3(data.radius * std::sin(data.angle), 0, data.radius * std::cos(data.angle));

            data.cameraTransform -> position = vec3::lerp(data.cameraPosition, playerOffset, 1-std::pow(0.6, g_time.deltaTime * 30)) + target;
            data.cameraPosition = playerOffset;
            data.camera -> front = (data.violetTransform -> position - target - playerOffset).normalized();
            data.circularMotion = target;
        });

        // FIXED UPDATE (50 FPS)
        event.fixedUpdate([]
        (System& script)
        {
            Event& data = script.data<Event>();
            
            AnimationState& idle = data.animator -> get("idle");
            AnimationState& move = data.animator -> get("move");

            Vector3 camDirection = -data.camera -> front.xz().normalized();
            data.force = Vector3(g_keyboard.inputs[GLFW_KEY_D].held - g_keyboard.inputs[GLFW_KEY_A].held, 0, g_keyboard.inputs[GLFW_KEY_S].held - g_keyboard.inputs[GLFW_KEY_W].held).normalized();
            if(data.force == 0)
            {
                data.forceDirection = 0;
                data.direction = ((Vector3)(Quaternion(vec3::signedAngle(vec3::forward, data.bufferDirection), vec3::down) * Quaternion(vec3::signedAngle(data.lastCamera, camDirection), vec3::up))).normalized();
            }
            else
            {
                data.bufferDirection = data.direction;
                data.lastCamera = camDirection;
                data.direction = data.force;

                float theta = vec3::signedAngle(data.force * Vector3(-1, 1, 1), vec3::forward) + vec3::signedAngle(vec3::forward, camDirection);
                data.forceDirection = Vector3(std::sin(theta), 0, std::cos(theta));
            }

            data.angle = math::modf(data.angle + (data.lastMousePosition - g_window.mouseScreenPosition()).x * data.mouseSensitivity, 2*M_PI);
            data.lastMousePosition = g_window.mouseScreenPosition();

            data.animator -> setParameter("moving", data.force != vec3::zero);
            idle.setParameter("force", data.direction);
            move.setParameter("force", data.direction);
        });
    }
}