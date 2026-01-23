use ash::{vk, Entry};

fn main() {
    let entry = unsafe { Entry::load().expect("Failed to create a vulkan entry.\n") };
    let appInfo = vk::ApplicationInfo {
        api_version: vk::make_api_version(0, 1, 0, 0),
        ..Default::default()
    };
    let createInfo = vk::InstanceCreateInfo {
        p_application_info: &appInfo,
        ..Default::default()
    };
    let instance = unsafe { entry.create_instance(&createInfo, None).expect("Failed to create a vulkan instance.\n") };
    println!("Created instance.");
    unsafe {
        instance.destroy_instance(None);
    }
    println!("Instance destroyed.");
}
