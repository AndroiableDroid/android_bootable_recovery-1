#ifndef __TWRPSUPER_H
#define __TWRPSUPER_H
#include <stdlib.h>                   
#include <sys/mount.h>                
#include <unistd.h>                   
                                      
#include <chrono>                     
#include <memory>                     
#include <set>                        
#include <string>                     
#include <vector>                     
                                      
#include <android-base/chrono_utils.h>
#include <android-base/file.h>        
#include <android-base/logging.h>     
#include <android-base/strings.h>     
#include <fs_avb/fs_avb.h>            
#include <fs_mgr.h>                   
#include <fs_mgr_dm_linear.h>         
#include <fs_mgr_overlayfs.h>         
#include <libgsi/libgsi.h>            
#include <liblp/liblp.h>
#include "uevent.h"
#include "partitions.hpp"
#include "twcommon.h"
#include "devices.h"
#include "uevent_listener.h"

using android::base::Split;
using android::base::Timer;
using android::fs_mgr::AvbHandle;
using android::fs_mgr::AvbHandleStatus;
using android::fs_mgr::AvbHashtreeResult;
using android::fs_mgr::AvbUniquePtr;
using android::fs_mgr::BuildGsiSystemFstabEntry;
using android::fs_mgr::Fstab;
using android::fs_mgr::FstabEntry;
using android::fs_mgr::ReadDefaultFstab;
using android::fs_mgr::ReadFstabFromDt;
using android::fs_mgr::SkipMountingPartitions;
using android::init::Uevent;
using namespace std::literals;

class twrp_super {
public:
    twrp_super();
    bool MountPartition(TWPartition* twrpPartition);
    bool InitMappedDevice(const std::string& verity_device);
protected:
    std::unique_ptr<android::init::DeviceHandler> device_handler_;
    android::init::UeventListener uevent_listener_;
};

#endif
