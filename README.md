# Project Burst
A under-developing toy ray tracer based on [RayTracingInOneWeekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html).

## Build
This project uses [Xmake](https://xmake.io/).
### Build Binaries
Simply type `xmake` at root directory of the project.
### Generate Visual Studio Project
```powershell
xmake project -k vsxmake -m "debug,release" -a "x64"
```
