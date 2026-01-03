# Engine Cheatsheet

## Core

```cpp
Game Game(const u32 windowWidth, const u32 windowHeight, const char* windowTitle);
```
IInitializes the whole engine.

```cpp
template<typename T, typename... Args>
void Game::SetFirstScene<T>(const char* name, Args&&... args);
```
Creates the first scene of type `T` with a given name and passes its args to the scene.

```cpp
void Game::Run(const u32 targetFps, const u32 updateFrequency, const u8 maxUpdatesPerFrame = 5);
```
Starts the game loop with a target rendering fps, update loop frequency and the maximum amount of update loops per frame (blocking).

---

## Scenes

```cpp
class Scene;
```
Base scene class from which all scenes are built upon.

```cpp
Scene(const Context& context);
```
The context must be always be first and passed to the base class.

```cpp
void Scene::Update(const float deltaT);
```
Scenes must have a Update method taking in the frameTime in second.

```cpp
void Scene::Draw();
```
Scenes must have a Draw method.

```cpp
void Scene::OnEnter();
```
Scenes must have a OnEnter method that is triggered whenever the scene is entered.

```cpp
void Scene::OnExit();
```
Scenes must have a OnExit method that is triggered whenever the scene is exited.

---

## Context

```cpp
struct Context;
```
A struct that provides references to all the following systems and should be passed when needed (by default given to scenes)

```cpp
double Context::updateTime
```
Average update time in milliseconds

```cpp
double Context::drawTime
```
Average draw time in milliseconds

---

## Scene Manager

```cpp
template<typename T, typename... Args>
void SceneManager::AddScene<T>(const char* name, Args&&... args);
```
Adds a scene of type `T` to the manager with a given name and passes its args to the scene.

```cpp
void SceneManager::RemoveScene(const char* name);
```
Deletes the scene with the given name.

```cpp
void SceneManager::ChangeScene(const char* name);
```
Exits the current scene and enters the scene with given name.

---

## Entity Component System (ECS)

Uses **EnTT**  
https://github.com/skypjack/entt/wiki

```cpp
entt::entity entt::Registry::create();
```
Creates a new entity.

```cpp
template<typename T, typename... Args>
T& entt::Registry::emplace<T>(entt::entity entity, Args&&... args);
```
Adds a component of type `T` to the given entity with the component args passed.

```cpp
template<typename T>
T& entt::Registry::get<T>(entt::entity entity);
```
Gets a reference to the component of type `T` of the given entity.

```cpp
template<typename... T>
auto entt::Registry::view<T...>();
```
Given a list of component types returns a iterable list of entities with all said components.

```cpp
template<typename... T, typename... V>
auto entt::Registry::group<T...>(entt::get<V...>);
```
A more efficient view for iteration that is based around a main component.

```cpp
template<typename... T>
for (auto [entity, T...] : (view/group).each())
```
An easy we to iterate through all entities and their components as references.

---

## Event System

```cpp
entt::Dispatcher.sink<T>().connect<F>(ptr);
```
Given an event type `T` links a function F or method to an event (this pointer must be passed for methods).

```cpp
template<typename T, typename... Args>
void entt::Dispatcher.trigger<T>(Args&&... args);
```
Sends out an event of type `T` with given args.

---

## Renderer

```cpp
class Renderer;
```
If an entity has a Sprite and transform component it is drawn automatically by the renderer.

---

## Systems

```cpp
class System;
```
Base system class, systems are independent of scenes and are updated and drawn every frame.

```cpp
System(const Context& context);
```
The context must be always be first and passed to the base class.

```cpp
void System::Update(const float deltaT);
```
Systems must have a Update method taking in the frameTime in seconds.

```cpp
void System::Draw();
```
Systems must have a Draw method.

---

## System Manager

```cpp
template<typename T, typename... Args>
T& AddSystem<T>(const u32 priority = 0, Args&&... args);
```
Adds a system of type `T` to the manager with a priority (smaller = lower priority) and args passed.

---

## Resource Manager

```cpp
template<typename T, typename... Args>
ResourceCache<T>& ResourceManager::AddCache<T>(Args&&... args);
```
Creates a resource cache that store type `T`, the args provided must be a load function that returns `T` and takes a path, and an unload function taking in `T`.

```cpp
template<typename T>
ResourceCache<T>& ResourceManager::GetCache<T>();
```
Returns the cache for type `T`.

```cpp
template<typename T>
void ResourceManager::RemoveCache();
```
Removes and unloads the cache for type `T`.

```cpp
void ResourceManager::ClearCaches();
```
Removes and unloads all caches.

---

## Resource Cache

```cpp
template<typename T>
class ResourceCache;
```
A cache of a certain type `T`, using the provided load and unload functions when created in the manager.

```cpp
T& ResourceCache::Get(const char* path);
```
Loads type `T` into the cache using its load function and the path given.

```cpp
T& ResourceCache::Add(const T& object, const char* name);
```
Manually adds an already loaded resource to the cache.

```cpp
void ResourceCache::Remove(const char* path);
```
Unloads any cached object with given path.

---

## Lua Manager

```cpp
class LuaManager;
```
Runs all scripts each frame by calling their "Update" function and passing the frameTime as the only parameter.

```cpp
template<typename T>
void RegisterEvent();
```
Registers an event type to be sent to all scripts with a function called "On" + *struct name* + "Event" that takes in the event.

```cpp
bool LuaManager::LoadScript(const char* path);
```
Loads a lua script from the given path.

```cpp
bool LuaManager::RemoveScript(const char* path);
```
Removes the scripts loaded from given path.

```cpp
void LuaManager::EnableScript(const char* path);
void LuaManager::DisableScript(const char* path);
```
Enables or disables the associated script with that path.

```cpp
void LuaManager::ReloadScripts();
```
Reloads all scripts.

---

## Network Manager

```cpp
bool NetworkManager::InitServer(const u16 port, const u32 maxPeers = 64, const u32 channels = 1, const u32 timeoutMs = 0);
```
Creates a server at a defined port (can connect and be connected to).

```cpp
bool NetworkManager::InitClient(const u32 channels = 1, const u32 timeoutMs = 0);
```
Creates a client (can only connect).

```cpp
void NetworkManager::Shutdown();
```
Closes either server or client and waits for thread to join.

```cpp
PeerId NetworkManager::Connect(const Address& address, const u32 data = 0);
```
Attempts to connect to an address with an optional u32 of data possible.

```cpp
void NetworkManager::Disconnect(const PeerId peer, const u32 data = 0);
```
Disconnects a peer with an optional u32 of data possible.

```cpp
void NetworkManager::Send(const PeerId peer, const std::vector<u8>& data, const ChannelId channel = 0, const bool reliable = true);
```
Sends a packet to a peer.

```cpp
std::queue<NetworkEvent> NetworkManager::Poll();
```
Retrieves pending network events.

```cpp
Peer NetworkManager::GetPeer(const PeerId peer);
```
Returns a valid peer struct for a peerId.

---

## Logger

```cpp
void Logger::SetLogLevel(const LogLevel level);
```
Sets the minimum log level of text and file output.

```cpp
void Logger::SetLogFile(const char* path);
```
Sets the output file path (empty path closes file).

```cpp
template<typename... Args>
void Logger::Write(const LogLevel level, Args&&... args);
```
Logs at the given level with args being any inputs separated by commas.

```cpp
void  LogDebug(Args&&... args);
```
Debug shortcut (depends on current scope having `_context.logger` available)