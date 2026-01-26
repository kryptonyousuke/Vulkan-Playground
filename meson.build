project(
  'Vulkan Triangle',
  'C',
  version:'1.0',
  default_options: [
    'optimization=3'
])

# Needed programs
cargo = find_program('cargo')
glslc = find_program('glslc')

rust_path = 'src/playground/rust/'


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
vulkan_dep = vulkan.partial_dependency(
  includes: true,
  link_args: true)
vulkan_headers = vulkan.partial_dependency(includes: true)
sdl3 = dependency('sdl3')


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

oh_my_vulkan_dep = declare_dependency(
  link_with: oh_my_vulkan,
  include_directories: inc,
  dependencies: vulkan_dep)

vulkan_triangle_src = [
  # main program
  'src/playground/vulkan_triangle.c',
  # utilities
  'src/utils/SDL_UTILS/SDL3_UTILS.c',
  'src/utils/sys_interaction.c',
]


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
vulkan_triangle_deps = [vulkan_dep, sdl3, oh_my_vulkan_dep]
vulkan_triangle_rs = custom_target(
  'vulkan_triangle_rs',
  command: [
    cargo,
    'build',
    '--release',
    '--target-dir', meson.current_build_dir(),
    '--manifest-path', meson.project_source_root() / rust_path / 'vulkan_triangle/Cargo.toml',
  ],
  output: 'vulkan_triangle_rs',
  build_always_stale: true,
  console: true,
  install: true,
  install_dir: get_option('bindir'),
)



rust_targets = vulkan_triangle_rs
all_targets = shader_targets

executable('VulkanTriangle', vulkan_triangle_src, all_targets, 
          include_directories: inc, dependencies: vulkan_triangle_deps, install: true)