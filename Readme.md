# [Clickup Project](https://app.clickup.com/36938761/v/l/4-60960723-1)

# COAD
Light-weight game engine written in CPP.

## Naming convertion
- Struct and typedefs: PascalCase
- Function parameters: snake_case
- Struct members: snake_case
- Function names: camelCase
- Constants and defines: ALL_UPPER

## Language features used
- Function overload
- POD structs
- Free functions
- Templates for containers or functions. (still pod for templated structs)

## Policies
- Functions always return a Result type value
- Concept compatibility function names. i.e.: arrayCreate, arrayDestroy.

## Third-party libraries
- Core
- Audio
  - FMOD
- Render
  - DX11
  - Vulkan
- Physics
  - Bullet
- Networking
  - ENet
