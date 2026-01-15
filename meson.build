project(
        'Vulkan Triangle',
        'C',
        version:'1.0')
inc = include_directories('include')
lib_vulkan = dependency('vulkan')
lib_sdl3 = dependency('sdl3')
dependencies = [lib_vulkan, lib_sdl3]
vulkan_triangle_src = [
        'src/vulkan_triangle.c',
        'src/vk_utils.c',
        'src/SDL_UTILS/SDL3_UTILS.c',
        'src/sys_interaction.c',
        'src/vk_swapchain.c',
        'src/vk_instance.c',
        'src/vk_pipeline.c'
]

glslc = find_program('glslc')
shaders_dir = './shaders'
shaders = [
        'mesh_basic.frag',
        'mesh_basic.vert'
]
shader_targets = []
foreach shader_name : shaders
        build_name = shader_name + '.spv'
        shader_targets += custom_target('shader_' + shader_name,
                input: shaders_dir + '/' + shader_name,
                output: build_name,
                command: [glslc,'-O', '@INPUT@', '-o', '@OUTPUT@'],
                install: true,
                install_dir: shaders_dir)
endforeach
executable('VulkanTriangle', vulkan_triangle_src, shader_targets, include_directories: inc, dependencies: dependencies, install: true, install_dir: '')

