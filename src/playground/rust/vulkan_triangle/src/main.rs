use ash::{vk, Entry};

fn main() {
    let entry = unsafe { Entry::load().expect("Failed to create a vulkan entry.\n") };
    let app_info = vk::ApplicationInfo {
        api_version: vk::make_api_version(0, 1, 0, 0),
        ..Default::default()
    };
    let create_info = vk::InstanceCreateInfo {
        p_application_info: &app_info,
        ..Default::default()
    };
    let available_instance_extension = unsafe { entry.enumerate_instance_extension_properties(None).expect("Failed to get available instance extensions.") };
    println!("Available extensions:");
    for ext in &available_instance_extension {
        println!("      {} (version {})", ext.extension_name_as_c_str().expect("Failed to get instance name.").to_string_lossy(), ext.spec_version);
    }
    let instance = unsafe { entry.create_instance(&create_info, None).expect("Failed to create a vulkan instance.\n") };
    println!("Created instance.");
    unsafe {
        instance.destroy_instance(None);
    }
    println!("Instance destroyed.");
}
