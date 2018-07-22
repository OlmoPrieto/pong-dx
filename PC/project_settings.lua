#!lua

pd = "../Project"

-- A solution contains projects, and defines the available configurations
solution "Project"
  configurations { "Debug", "Release" }
  platforms { "native", "x64", "x32" }
  location ( "./" ) -- Location of the solutions
         
  -- Project
  project "Project"
    kind "ConsoleApp"
    language "C++"
    location ( pd .. "/" ) 
    targetdir (pd .. "/bin/")

    --buildoptions("-stdlib=libstdc++")
    buildoptions_cpp("-std=c++14")
  
    buildoptions_objcpp("-std=c++14")
      
    -- includedirs { 
    --   "./Project/include", 
    --   "./Project/dependencies",
    --   "./Project/dependencies/glew/include/",
    --   "./Project/dependencies/GLFW/deps/",
    -- }
    includedirs { 
      pd .. "/include", 
      pd .. "/dependencies",
      pd .. "/dependencies/glew/include/",
      pd .. "/dependencies/GLFW/deps/",
    }
      
    -- INCLUDE FILES
    files { -- GLEW
      --group = "GLEW", "./Project/dependencies/glew/include/glew.c", 
      group = "GLEW", pd .. "/dependencies/glew/include/glew.c", 
      --"./Project/dependencies/glew/include/GL/glew.h"
    }
    
    -- files{ group = "include", "./Project/include/**.h" } -- include filter and get the files
    -- files{ group = "src", "./Project/src/**.cc", "./Project/src/**.cpp" } -- src filter and get the files
    files{ group = "include", pd .."/include/**.h" } -- include filter and get the files
    files{ group = "src", pd .."/src/**.cc", pd .."/src/**.cpp" } -- src filter and get the files
    excludes {
      -- temporal exclude until windows support for sockets is implemented
      pd .. "/include/sockets.h",
      pd .. "/src/sockets.cpp"
    }

    -- only when compiling as library
    --defines { "GLEW_STATIC" }
  
    configuration { "windows" }
      files {  -- GLFW
      --   group = "GLFW", "./Project/dependencies/GLFW/src/context.c", 
      --     "./Project/dependencies/GLFW/src/init.c", 
      --     "./Project/dependencies/GLFW/src/input.c",
      --     "./Project/dependencies/GLFW/src/monitor.c",
      --     "./Project/dependencies/GLFW/src/wgl_context.c",
      --     "./Project/dependencies/GLFW/src/win32_init.c",
      --     "./Project/dependencies/GLFW/src/win32_monitor.c",
      --     "./Project/dependencies/GLFW/src/win32_time.c",
      --     "./Project/dependencies/GLFW/src/win32_tls.c",
      --     "./Project/dependencies/GLFW/src/win32_window.c",
      --     "./Project/dependencies/GLFW/src/window.c",
      --     "./Project/dependencies/GLFW/src/winmm_joystick.c",
      --   --"./Project/dependencies/GLFW/include/GLFW/glfw3.h"
      -- }
        group = "GLFW", 
          pd .. "/dependencies/GLFW/src/context.c", 
          pd .. "/dependencies/GLFW/src/init.c", 
          pd .. "/dependencies/GLFW/src/input.c",
          pd .. "/dependencies/GLFW/src/monitor.c",
          pd .. "/dependencies/GLFW/src/wgl_context.c",
          pd .. "/dependencies/GLFW/src/win32_init.c",
          pd .. "/dependencies/GLFW/src/win32_monitor.c",
          pd .. "/dependencies/GLFW/src/win32_time.c",
          pd .. "/dependencies/GLFW/src/win32_tls.c",
          pd .. "/dependencies/GLFW/src/win32_window.c",
          pd .. "/dependencies/GLFW/src/window.c",
          pd .. "/dependencies/GLFW/src/winmm_joystick.c",
        --pd .. "dependencies/GLFW/include/GLFW/glfw3.h"
      }
      
      links {
        "opengl32"
      }
      defines { "__PLATFORM_WINDOWS__","_GLFW_WIN32", "_GLFW_WGL", "_GLFW_USE_OPENGL" }
      buildoptions_cpp("/Y-")
      windowstargetplatformversion "10.0.15063.0"
       
    configuration { "macosx" }
      files {  -- GLFW
    --     group = "GLFW", "./Project/dependencies/GLFW/src/context.c", 
    --       "./Project/dependencies/GLFW/src/init.c", 
    --       "./Project/dependencies/GLFW/src/input.c",
    --       "./Project/dependencies/GLFW/src/monitor.c",
    --       "./Project/dependencies/GLFW/src/nsgl_context.m",
    --       "./Project/dependencies/GLFW/src/cocoa_init.m",
    --       "./Project/dependencies/GLFW/src/cocoa_monitor.m",
    --       "./Project/dependencies/GLFW/src/mach_time.c",
    --       "./Project/dependencies/GLFW/src/posix_tls.c",
    --       "./Project/dependencies/GLFW/src/cocoa_window.m",
    --       "./Project/dependencies/GLFW/src/window.c",
    --       "./Project/dependencies/GLFW/src/iokit_joystick.m",
    --     --"./Project/dependencies/GLFW/include/GLFW/glfw3.h"
    -- }
        group = "GLFW", 
          pd .. "/dependencies/GLFW/src/context.c", 
          pd .. "/dependencies/GLFW/src/init.c", 
          pd .. "/dependencies/GLFW/src/input.c",
          pd .. "/dependencies/GLFW/src/monitor.c",
          pd .. "/dependencies/GLFW/src/nsgl_context.m",
          pd .. "/dependencies/GLFW/src/cocoa_init.m",
          pd .. "/dependencies/GLFW/src/cocoa_monitor.m",
          pd .. "/dependencies/GLFW/src/mach_time.c",
          pd .. "/dependencies/GLFW/src/posix_tls.c",
          pd .. "/dependencies/GLFW/src/cocoa_window.m",
          pd .. "/dependencies/GLFW/src/window.c",
          pd .. "/dependencies/GLFW/src/iokit_joystick.m",
        --pd .. "/dependencies/GLFW/include/GLFW/glfw3.h"
    }
    links  {
      "Cocoa.framework", "OpenGL.framework", "IOKit.framework", "CoreVideo.framework",
    }
    linkoptions { "-framework Cocoa","-framework QuartzCore", "-framework OpenGL", "-framework OpenAL" }
    defines { "__PLATFORM_MACOSX__", "_GLFW_COCOA", "_GLFW_NSGL", "_GLFW_USE_OPENGL" }
       
    configuration { "linux" }
      files {  -- GLFW
        group = "GLFW", 
        	pd .. "/dependencies/GLFW/src/context.c", 
          pd .. "/dependencies/GLFW/src/init.c", 
          pd .. "/dependencies/GLFW/src/input.c",
          pd .. "/dependencies/GLFW/src/monitor.c",
          pd .. "/dependencies/GLFW/src/glx_context.c",
          pd .. "/dependencies/GLFW/src/x11_init.c",
          pd .. "/dependencies/GLFW/src/x11_monitor.c",
          pd .. "/dependencies/GLFW/src/posix_time.c",
          pd .. "/dependencies/GLFW/src/posix_tls.c",
          pd .. "/dependencies/GLFW/src/x11_window.c",
          pd .. "/dependencies/GLFW/src/window.c",
          pd .. "/dependencies/GLFW/src/linux_joystick.c",
          pd .. "/dependencies/GLFW/src/xkb_unicode.c",
        --pd .. "/dependencies/GLFW/include/GLFW/glfw3.h"
    }
    links {
      "X11", "Xrandr", "Xcursor", "Xinerama", "Xi", "Xxf86vm", "rt", "pthread", "GL", "glut", "GLU", "m"
    }
    includedirs {
      "/usr/include/GL/"
    }
    libdirs {
      "/usr/bin/",
      "/usr/lib/"
    }
    defines { "__PLATFORM_LINUX__", "_GLFW_X11", "_GLFW_HAS_GLXGETPROCADDRESS", "_GLFW_GLX", "_GLFW_USE_OPENGL" }

    configuration "debug"
      defines { "DEBUG" }
      flags { "Symbols", "ExtraWarnings"}

    configuration "release"
      defines { "NDEBUG" }
      flags { "Optimize", "ExtraWarnings" }