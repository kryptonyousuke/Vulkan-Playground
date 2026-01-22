project(
        'Vulkan Triangle',
        'C',
        version:'1.0',
        default_options: [
          'optimization=3'
        ])
arch = get_option('target_arch')
args = []
if get_option('enabledebug')
  args += '-DENABLED_DEBUG'
  message('Debug mode enabled.')
endif

# The default value is native.
# It does compile using specific special instructions like AVX2 (SIMD). 
# Native value detects automatically the supported instructions.
# Can be setted manually if you want to compile for another machine.
add_project_arguments('-march=' + arch, language : 'c') 

inc = include_directories('include')
vulkan = dependency('vulkan')
sdl3 = dependency('sdl3')
dependencies = [vulkan, sdl3]

oh_my_vulkan = static_library('OhMyVulkan', sources: [
        'src/OhMyVulkan/vk_utils.c',
        'src/OhMyVulkan/vk_swapchain.c',
        'src/OhMyVulkan/vk_instance.c',
        'src/OhMyVulkan/vk_pipeline.c',
        'src/OhMyVulkan/vk_physical_device.c',
        'src/OhMyVulkan/vk_logical_device.c',
        'src/OhMyVulkan/vk_mem.c',
        'src/OhMyVulkan/vk_sync.c',
], c_args: args)

vulkan_triangle_src = [
        # main program
        'src/playground/vulkan_triangle.c',
        # utilities
        'src/utils/SDL_UTILS/SDL3_UTILS.c',
        'src/utils/sys_interaction.c',
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
executable('VulkanTriangle', vulkan_triangle_src, shader_targets, link_with: oh_my_vulkan, include_directories: inc, dependencies: dependencies, install: true, install_dir: '')

