#include "vellichor.h"

void createWorld(const Material& objectDefault)
{
    Mesh& square = mesh::get("square");
    Mesh& cube = mesh::get("cube");

    Material objectDull = Material
    (shader::get("obj_shader"), [] (Entity entity, const Model& model, const Camera& camera, const Transform& cameraTransform)
    {
        Transform& transform = object::getComponent<Transform>(entity);
        Shader shader = model.material.shader;
        shader.use();
        
        shader.setMat4("model", (mat4x4(1).rotated(transform.rotation).translated(transform.position)).matrix, true);
        shader.setMat4("view", camera.view.matrix, true);
        shader.setMat4("projection", camera.projection.matrix, true);
        shader.setVec3("scale", transform.scale);
        
        shader.setVec3("lightPos", Vector3(-60, 20, -20));
        shader.setVec3("viewPos", cameraTransform.position);
        shader.setVec4("objColor", model.color);

        shader.setBool("advanced", false);
        shader.setFloat("material.shininess", 8);
        shader.setVec3("material.ambientStrength", 1);
        shader.setVec3("material.diffuseStrength", 1);
        shader.setVec3("material.specularStrength", 0.1f);
    });
    Material simple = Material(shader::get("simple_shader"));

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

    Object shelf("shelf", false);
    shelf.addComponent<Transform>(Transform(Vector3(5, -16.5f, start), 1));
    shelf.addComponent<Model>({color::WHITE, objectDefault, shelfMesh, texture::get("Vellichor/bookcase_texture.png")});
    shelf.addComponent<MeshAddon>(
    MeshAddon
    ({
        Vector3(91, 0, 0), Vector3(0, 0, -25), Vector3(91, 0, -25), Vector3(0, 0, -50), Vector3(91, 0, -50), Vector3(0, 0, -75), Vector3(91, 0, -75)
    }));

    Object window("window", false);
    window.addComponent<Transform>(Transform(Vector3(-9.0f, -2.5f, start), 1));
    window.addComponent<Model>({color::WHITE * 0.8f, simple, winMesh, texture::get("Vellichor/window_texture.png")});
    window.addComponent<MeshAddon>(
    MeshAddon
    ({
        {Vector3(119, 0, 0), Quaternion(math::radians(180), vec3::up)}, Vector3(0, 0, -25), {Vector3(119, 0, -25), Quaternion(math::radians(180), vec3::up)}, Vector3(0, 0, -50), 
        {Vector3(119, 0, -50), Quaternion(math::radians(180), vec3::up)}, Vector3(0, 0, -75), {Vector3(119, 0, -75), Quaternion(math::radians(180), vec3::up)}
    }));

    Object back("back", false);
    back.addComponent<Transform>(Transform(Vector3(-20.0f, 5, 0), Vector3(180, 60, 1), Quaternion(math::radians(90), vec3::up)));
    back.addComponent<Model>({Color(0.04f, 0.04f, 0.1f, 1), simple, square, texture::get("default.png")});
    back.addComponent<MeshAddon>(MeshAddon({{Vector3(140, 0, 0), Quaternion(math::radians(180), vec3::up)}}));

    Object pane("windowPane", false);
    pane.addComponent<Transform>(Transform(Vector3(-9.45f, 5, start), 7.25f, Quaternion(math::radians(90), vec3::up)));
    pane.addComponent<Model>({color::CLEAR, objectDefault, square, texture::get("default.png")});
    pane.addComponent<MeshAddon>(
    MeshAddon
    ({
        {Vector3(119.9f, 0, 0), Quaternion(math::radians(180), vec3::up)}, Vector3(0, 0, -25), {Vector3(119.9f, 0, -25), Quaternion(math::radians(180), vec3::up)}, Vector3(0, 0, -50), 
        {Vector3(119.9f, 0, -50), Quaternion(math::radians(180), vec3::up)}, Vector3(0, 0, -75), {Vector3(119.9f, 0, -75), Quaternion(math::radians(180), vec3::up)}
    }));

    Object ladder("ladder", false);
    ladder.addComponent<Transform>(Transform(Vector3(-4.9125f, -16.5f, 5.25f+start), Quaternion(math::radians(45), vec3::up)));
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
            if(child.name.find("windowLight") != std::string::npos)
            {
                childTransform.position += Vector3(30.0f, 0, 0);
                child.getComponent<SpotLight>().direction *= Vector3(-1, 1, -1);
            }
        }
    }

    Object wall("wall");
    wall.addComponent<Transform>(Transform(Vector3(50.5f, -5, 60), 1 , Quaternion(math::radians(180), vec3::up)));
    wall.addComponent<Model>(Model {0.75f, simple, wallMesh, texture::get("Vellichor/wall_texture.png")});

    Object wall2 = wall.clone();
    wall2.getComponent<Transform>() = Transform(Vector3(-9.5f, -5, 0), 1 , Quaternion(math::radians(90), vec3::up));
    wall2.getComponent<Model>().color *= 1.15f;

    Object wall3 = wall.clone();
    wall3.getComponent<Transform>() = Transform(Vector3(110.5f, -5, 0), 1 , Quaternion(math::radians(90), vec3::down));
    wall3.getComponent<Model>().color *= 1.3f;

    Object wall4 = wall.clone();
    wall4.getComponent<Transform>() = Transform(Vector3(50.5f, -5, -60), 1);
    wall4.getComponent<Model>().color *= 1.45f;

    Object floor("floor");
    floor.addComponent<Transform>(Transform(Vector3(50.5f, -32.5f, 0), 120, Quaternion(math::radians(90), vec3::left)));
    floor.addComponent<Model>({Color(0.2f, 0.02f, 0.03f, 1), simple, shape::square(24), texture::get("Vellichor/carpet_texture.png")});

    Object ceiling("ceiling");
    ceiling.addComponent<Transform>(Transform(Vector3(50.5f, 19.5f, 0), 120, Quaternion(math::radians(90), vec3::right)));
    ceiling.addComponent<Model>({Color(0.1f, 0.0375f, 0.0375f, 1), simple, shape::square(24), texture::get("default.png")});
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
    idle.add(Animation(texture::get("Vellichor/Violet/Idle/violet_idle", {"0000", "0001", "0002"}, texture::PNG), idleFrameCount), "south");
    idle.add(Animation(texture::get("Vellichor/Violet/Idle/violet_idle", {"0003", "0004", "0005"}, texture::PNG), idleFrameCount), "swest");
    idle.add(Animation(texture::get("Vellichor/Violet/Idle/violet_idle", {"0006", "0007", "0008"}, texture::PNG), idleFrameCount), "west");
    idle.add(Animation(texture::get("Vellichor/Violet/Idle/violet_idle", {"0009", "0010", "0011"}, texture::PNG), idleFrameCount), "nwest");
    idle.add(Animation(texture::get("Vellichor/Violet/Idle/violet_idle", {"0012", "0013", "0014"}, texture::PNG), idleFrameCount), "north");
    idle.add(Animation(texture::get("Vellichor/Violet/Idle/violet_idle", {"0015", "0016", "0017"}, texture::PNG), idleFrameCount), "neast");
    idle.add(Animation(texture::get("Vellichor/Violet/Idle/violet_idle", {"0018", "0019", "0020"}, texture::PNG), idleFrameCount), "east");
    idle.add(Animation(texture::get("Vellichor/Violet/Idle/violet_idle", {"0021", "0022", "0023"}, texture::PNG), idleFrameCount), "seast");
    register8DStates(idle);

    int moveFrameCount = 5;
    move.add(Animation(texture::get("Vellichor/Violet/Move/violet_move00", {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09"}, texture::PNG), moveFrameCount), "south");
    move.add(Animation(texture::get("Vellichor/Violet/Move/violet_move00", {"10", "11", "12", "13", "14", "15", "16", "17", "18", "19"}, texture::PNG), moveFrameCount), "swest");
    move.add(Animation(texture::get("Vellichor/Violet/Move/violet_move00", {"20"}, texture::PNG), moveFrameCount), "west");
    move.add(Animation(texture::get("Vellichor/Violet/Move/violet_move00", {"21"}, texture::PNG), moveFrameCount), "nwest");
    move.add(Animation(texture::get("Vellichor/Violet/Move/violet_move00", {"22"}, texture::PNG), moveFrameCount), "north");
    move.add(Animation(texture::get("Vellichor/Violet/Move/violet_move00", {"23"}, texture::PNG), moveFrameCount), "neast");
    move.add(Animation(texture::get("Vellichor/Violet/Move/violet_move00", {"24"}, texture::PNG), moveFrameCount), "east");
    move.add(Animation(texture::get("Vellichor/Violet/Move/violet_move00", {"25"}, texture::PNG), moveFrameCount), "seast");
    register8DStates(move);

    animator.addTransition<bool>("moving", Operation::equals, true, std::pair{"idle", "move"});
    animator.addTransition<bool>("moving", Operation::equals, false, std::pair{"move", "idle"});

    return animator;
}


void vellichor::initialize()
{
    struct Event : Script
    {
        Object defaultCamera, test;

        Animator *animator;
        Camera *camera;
        Transform *violetTransform, *cameraTransform, *spotTransform;
        
        Vector2 lastMousePosition;
        Vector3 cameraPosition, bufferDirection = vec3::forward, lastCamera = vec3::forward, direction = vec3::forward, force, forceDirection;

        float speed = 2.0f, rotationalSpeed = 0, radius = 6, angle = 0, mouseSensitivity = 0.5f, lastDelta;
        key::KeyArray forward = {key::W}, back = {key::S}, left = {key::A}, right = {key::D};
    };
    Event& event = object::initializeScript<Event>();
    {
        // LOAD
        event.load([]
        (System& script)
        {
            texture::load("Vellichor/", {"wall_texture", "bookcase_texture", "ladder_texture", "carpet_texture", "window_texture"}, texture::PNG);
            texture::load("Vellichor/Violet/Idle/violet_idle00", {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"}, texture::PNG);
            texture::load("Vellichor/Violet/Move/violet_move00", {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25"}, texture::PNG);
            texture::load("Vellichor/Mother/Idle/mother_idle00", {"00"}, texture::PNG);

            Event& data = script.data<Event>();
            data.lastMousePosition = window::cursorScreenPosition();

            Material playerMaterial = 
            Material (shader::get("obj_shader"), [] (Entity entity, const Model& model, const Camera& camera, const Transform& cameraTransform)
            {
                shader::advanced(entity, model, camera, cameraTransform, 1, 0.2f, 0.1f, 64);
            });
            Material objectDefault = 
            Material(shader::get("obj_shader"), [] (Entity entity, const Model& model, const Camera& camera, const Transform& cameraTransform)
            {
                shader::advanced(entity, model, camera, cameraTransform, 1, 1, 0.5f, 32);
            });
            Material uiMaterial = Material {shader::get("ui_shader"), shader::ui};

            createWorld(objectDefault);

            // Object mother("mother");
            // mother.addComponent<Transform>(Transform(Vector3(0, 2.5f, 15), Vector3(5.0f, 5.0f, 0), Quaternion(math::radians(90), vec3::up)));
            // mother.addComponent<Model>({color::WHITE, playerMat, mesh::get("square"), texture::get("Vellichor/Mother/Idle/mother_idle0000.png")});

            Object violet("violet");
            violet.addComponent<Model>({color::WHITE, playerMaterial, mesh::get("square"), texture::get("Vellichor/Violet/Idle/violet_idle0000.png")});
            violet.addComponent<BoxCollider>(BoxCollider(true)).setTrigger([]
            (Entity entity, Entity compare, bool edge, int triggered)
            {
                float climbSpeed = 2, maxHeight = 0.6f, minHeight = -31.5f;
                if(key::held(key::SPACE))
                {
                    Transform &transform = object::getComponent<Transform>(entity);
                    Vector3 newTransform = transform.position + Vector3(0, climbSpeed * event::delta(), 0);
                    transform.position = Vector3(newTransform.x, math::clamp(newTransform.y, minHeight, maxHeight), newTransform.z);
                }
                if(key::held(key::LEFT_SHIFT))
                {
                    Transform &transform = object::getComponent<Transform>(entity);
                    Vector3 newTransform = transform.position - Vector3(0, climbSpeed * event::delta(), 0);
                    transform.position = Vector3(newTransform.x, math::clamp(newTransform.y, minHeight, maxHeight), newTransform.z);
                }
            });
            data.violetTransform = &violet.addComponent<Transform>(Transform(Vector3(0, 0.6f, 35), Vector3(1.125f, 2.25f, 1)));
            data.animator = &configurePlayerAnimator(violet);

            Object spotLight("follower");
            data.spotTransform = &spotLight.addComponent<Transform>(Transform(Vector3(object::find("violet").getComponent<Transform>().position + Vector3(0, 3, 0)), 0));
            spotLight.addComponent<Model>({color::CLEAR, objectDefault, mesh::get("cube")});
            spotLight.addComponent<SpotLight>({vec3::down, color::WHITE, 4.0f, object::brightness(2), std::cos(math::radians(25.0f)), std::cos(math::radians(20.0f))});

            Object camera("camera");
            data.camera = &camera.addComponent<Camera>(Camera(color::CLEAR, Vector3(0, -0.15, -1), vec3::up));
            data.cameraTransform = &camera.addComponent<Transform>(Transform{Vector3(data.violetTransform -> position + Vector3(0, 1.25f, 0))});
            data.cameraPosition =  data.cameraTransform -> position;
            data.defaultCamera = camera;
            window::setCamera(camera.data);

            Object cameraTwo = data.test = camera.clone();
            cameraTwo.getComponent<Transform>().position += Vector3(0, 0, data.radius);

            violet.addComponent<Billboard>(Billboard{camera.data});
            // mother.addComponent<Billboard>(Billboard{camera.data});


            // loadTTF("arial_mt_black/arial_mt_black.ttf");

            // Object curve("curveTest");
            // curve.addComponent<Rect>(Rect(Alignment(alignment::MIDDLE, alignment::CENTER)));
            // curve.addComponent<Spline>(Spline({Curve(Vector2(-1, 0), Vector2(1, 0), {})}, shader::get("spline_shader"), color::PURPLE));
        });

        // UPDATE
        event.update([]
        (System& script)
        {

            Event& data = script.data<Event>();

            float delta = data.lastDelta * 0.75 + event::delta() * 0.25;
            data.violetTransform -> position += data.forceDirection * data.speed * delta;
            data.spotTransform -> position = data.violetTransform -> position + Vector3(0, 3, 0);

            Vector3 playerOffset = data.violetTransform -> position + Vector3(0, 1.25f, 0);
            Vector3 target = Vector3(data.radius * std::sin(data.angle), 0, data.radius * std::cos(data.angle));

            data.cameraTransform -> position = vec3::lerp(data.cameraPosition, playerOffset, 1-std::pow(0.6, delta * 30)) + target;
            data.cameraPosition = playerOffset;
            data.camera -> front = (data.violetTransform -> position - target - playerOffset).normalized();

            if(key::pressed(key::Q))
            {
                window::setTitle("(◕‿◕✿)");
            }
            else if(key::pressed(key::E))
            {
                window::setTitle("ლ(ಠ益ಠლ)");
            }
            else if(key::pressed(key::TAB))
            {
                if(window::camera() == data.test.data)
                {
                    window::setCamera(data.defaultCamera.data);
                }
                else
                {
                    window::setCamera(data.test.data);
                }
            }

            if(window::camera() == data.test.data)
            {
                object::getComponent<Transform>(data.test.data).position += Vector3(key::held(key::RIGHT) - key::held(key::LEFT), key::held(key::SLASH) - key::held(key::PERIOD), key::held(key::DOWN) - key::held(key::UP)).normalized() * event::delta() * 20;
            }
            data.lastDelta = delta;
        });

        // FIXED UPDATE (50 FPS)
        event.fixedUpdate([]
        (System& script)
        {
            Event& data = script.data<Event>();
            
            AnimationState& idle = data.animator -> get("idle");
            AnimationState& move = data.animator -> get("move");

            Vector3 camDirection = -data.camera -> front.xz().normalized();
            data.force = Vector3(key::held(data.right) - key::held(data.left), 0, key::held(data.back) - key::held(data.forward)).normalized();
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

                float theta1 = vec3::signedAngle(data.force * Vector3(-1, 1, 1), vec3::forward), theta2 = vec3::signedAngle(vec3::forward, camDirection);
                data.forceDirection = Vector3(std::sin(theta1+theta2), 0, std::cos(theta1+theta2));
            }

            data.angle = math::modf(data.angle + (data.lastMousePosition - window::cursorScreenPosition()).x * data.mouseSensitivity, 2*M_PI);
            data.lastMousePosition = window::cursorScreenPosition();

            data.animator -> setParameter("moving", data.force != vec3::zero);
            idle.setParameter("force", data.direction);
            move.setParameter("force", data.direction);
        });
    }
}