#include "twrp_super.hpp"

twrp_super::twrp_super():uevent_listener_(16 * 1024 * 1024) {
    // auto boot_devices = android::fs_mgr::GetBootDevices();
    // device_handler_ = std::make_unique<android::init::DeviceHandler>(
            // std::vector<android::init::Permissions>{}, std::vector<android::init::SysfsPermissions>{}, std::vector<android::init::Subsystem>{},
            // std::move(boot_devices), false);
}

bool twrp_super::MountPartition(TWPartition* twrpPartition) {
    Fstab fstab;
    FstabEntry fstabEntry = {
        .blk_device = twrpPartition->Actual_Block_Device,
        .mount_point = twrpPartition->Get_Mount_Point(),
        .fs_type = twrpPartition->Current_File_System,
        .fs_mgr_flags.logical = twrpPartition->isSuper,
    };
    LOGINFO("attempting to mount %s", twrpPartition->Get_Mount_Point().c_str());

    fstab.emplace_back(fstabEntry);
    if (!fs_mgr_update_logical_partition(&fstabEntry)) {
        return false;
    }
    if (!InitMappedDevice(fstabEntry.blk_device)) {
        return false;
    }

    bool mounted = (fs_mgr_do_mount_one(fstabEntry) == 0);

    return mounted;
}

bool twrp_super::InitMappedDevice(const std::string& dm_device) {
    const std::string device_name(basename(dm_device.c_str()));
    const std::string syspath = "/sys/block/" + device_name;
    bool found = false;

    auto verity_callback = [&device_name, &dm_device, this, &found](const Uevent& uevent) {
        if (uevent.device_name == device_name) {
            LOG(VERBOSE) << "Creating device-mapper device : " << dm_device;
            device_handler_->HandleUevent(uevent);
            found = true;
            return android::init::ListenerAction::kStop;
        }
        return android::init::ListenerAction::kContinue;
    };

    uevent_listener_.RegenerateUeventsForPath(syspath, verity_callback);
    if (!found) {
        LOG(INFO) << "dm device '" << dm_device << "' not found in /sys, waiting for its uevent";
        Timer t;
        uevent_listener_.Poll(verity_callback, 10s);
        LOG(INFO) << "wait for dm device '" << dm_device << "' returned after " << t;
    }
    if (!found) {
        LOG(ERROR) << "dm device '" << dm_device << "' not found after polling timeout";
        return false;
    }

    return true;
}