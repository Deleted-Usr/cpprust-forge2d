(3 - 11 - 2026)

## Forge2D's Camera2D

The camera is a big part of any game, 2D or 3D. And so below are 4 proposed APIs, with small game code snippets. 

In each example, `player` and `boss` are existing game objects, and `renderer` is the `Renderer2D` render method. The
following code examples are written in C++:

1. Direct configuration on the camera
```c++
forge::Camera2D cam;

void onStart(forge::AssetManager& assetManager) override 
{
    cam.setZoom(2.0f);

    cam.setSmoothing(0.15f);          // Time in seconds.
    cam.setLookAhead({80.0f, 0.0f});  // Offset in world units.

    cam.follow(player);
}

void update(float deltaTime) override 
{
    if (bossFightStarted()) 
    {
        cam.follow(boss);
    }

    if (playerWasHit()) 
    {
        cam.shake(6.0f, 0.25f);      // Strength and duration.
    }

    cam.update(deltaTime);
}

void render() override 
{
    renderer.beginScene(cam);
    drawWorld(renderer);
    renderer.endScene();
}
```

2. Configuration structures
```c++
forge::Camera2D cam;

void onStart(forge::AssetManager& assetManager) override {
    forge::FollowSettings follow;
    follow.smoothingTime = 0.15f;
    follow.lookAhead = {80.0f, 0.0f};
    follow.deadZone = {24.0f, 16.0f};

    cam.setFollowSettings(follow);
    cam.follow(player);
}

void update(float deltaTime) override 
{
    if (bossFightStarted()) 
    {
        cam.follow(boss);
    }

    if (playerWasHit()) 
    {
        forge::ShakeSettings shake;
        shake.strength = 6.0f;
        shake.duration = 0.25f;
        shake.frequency = 25.0f;

        cam.shake(shake);
    }

    cam.update(deltaTime);
}
```

3. A separate camera controller
```c++
forge::Camera2D cam;
forge::CameraController2D cameraController{cam};

void onStart(forge::AssetManager& assetManager) override 
{
    cam.setZoom(2.0f);

    cameraController.setSmoothing(0.15f);
    cameraController.setLookAhead({80.0f, 0.0f});
    cameraController.follow(player);
}

void update(float deltaTime) override 
{
    if (bossFightStarted()) 
    {
        cameraController.follow(boss);
    }

    if (playerWasHit())
    {
        cameraController.shake(6.0f, 0.25f);
    }

    cameraController.update(deltaTime);
}

void render() override 
{
    renderer.beginScene(cam);
    drawWorld(renderer);
    renderer.endScene();
}
```

4. User-controlled movement with optional built-in effects
```c++
forge::Camera2D cam;

void onStart(forge::AssetManager& assetManager) override 
{
    cam.setZoom(2.0f);
    cam.setPosition(player.position());
}

void update(float deltaTime) override 
{
    auto cameraPosition = player.position();

    // This game's camera never goes below the floor.
    if (cameraPosition.y < 120.0f) 
    {
        cameraPosition.y = 120.0f;
    }

    cam.setPosition(cameraPosition);

    if (playerWasHit()) 
    {
        cam.shake(6.0f, 0.25f);
    }

    // Advances effects while preserving the position set above.
    cam.update(deltaTime);
}

void render() override 
{
    renderer.beginScene(cam);
    drawWorld(renderer);
    renderer.endScene();
}
```