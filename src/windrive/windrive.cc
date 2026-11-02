#include "assert.h"
#include "node_api.h"

#ifdef _WIN32
#include <windows.h>

#include <sstream>
#include <string>
#include <vector>

#define DRIVE_BUFFER_BYTE_LENGTH 256

struct LogicalDrive {
  std::string name;
  std::string driveType;
  bool hasSpace;
  DWORD bytesPerSect;
  DWORD freeClusters;
  DWORD totalClusters;
  double usedClusterPercent;
  double freeClusterPercent;
};

static const char *DriveTypeToString(UINT driveType) {
  switch (driveType) {
    case DRIVE_UNKNOWN:
      return "UNKNOWN";
    case DRIVE_NO_ROOT_DIR:
      return "NO_ROOT_DIR";
    case DRIVE_REMOVABLE:
      return "REMOVABLE";
    case DRIVE_FIXED:
      return "FIXED";
    case DRIVE_REMOTE:
      return "REMOTE";
    case DRIVE_CDROM:
      return "CDROM";
    case DRIVE_RAMDISK:
      return "RAMDISK";
    default:
      return "UNKNOWN";
  }
}

static bool CollectLogicalDrives(std::vector<LogicalDrive> *drives,
                                   std::string *error) {
  char buffer[DRIVE_BUFFER_BYTE_LENGTH];
  DWORD dwResult = GetLogicalDriveStringsA(DRIVE_BUFFER_BYTE_LENGTH, buffer);

  if (dwResult == 0) {
    *error =
        "Unable to find any logical drive. GetLogicalDriveStringsA() returned "
        "0.";
    return false;
  }
  if (dwResult > DRIVE_BUFFER_BYTE_LENGTH) {
    std::ostringstream oss;
    oss << "Insufficient buffer size (" << DRIVE_BUFFER_BYTE_LENGTH
        << "). GetLogicalDriveStringsA() returned " << dwResult << " bytes.";
    *error = oss.str();
    return false;
  }

  for (char *root = buffer; *root; root += strlen(root) + 1) {
    LogicalDrive drive;
    UINT type = GetDriveTypeA(root);

    drive.name = root;
    drive.driveType = DriveTypeToString(type);
    drive.hasSpace = false;

    if (type != DRIVE_CDROM) {
      DWORD sectPerClust;
      DWORD bytesPerSect;
      DWORD freeClusters;
      DWORD totalClusters;
      if (GetDiskFreeSpaceA(root, &sectPerClust, &bytesPerSect, &freeClusters,
                            &totalClusters) &&
          totalClusters > 0) {
        double freePercent =
            (static_cast<double>(freeClusters) / totalClusters) * 100.0;

        drive.hasSpace = true;
        drive.bytesPerSect = bytesPerSect;
        drive.freeClusters = freeClusters;
        drive.totalClusters = totalClusters;
        drive.freeClusterPercent = freePercent;
        drive.usedClusterPercent = 100.0 - freePercent;
      }
    }

    drives->push_back(drive);
  }

  return true;
}

static napi_value LogicalDriveToObject(napi_env env, const LogicalDrive &drive) {
  napi_status status;
  napi_value obj;

  status = napi_create_object(env, &obj);
  assert(status == napi_ok);

  napi_value name;
  status = napi_create_string_utf8(env, drive.name.c_str(), NAPI_AUTO_LENGTH,
                                   &name);
  assert(status == napi_ok);
  status = napi_set_named_property(env, obj, "name", name);
  assert(status == napi_ok);

  napi_value type;
  status = napi_create_string_utf8(env, drive.driveType.c_str(), NAPI_AUTO_LENGTH,
                                   &type);
  assert(status == napi_ok);
  status = napi_set_named_property(env, obj, "type", type);
  assert(status == napi_ok);

  if (drive.driveType != "CDROM" && drive.hasSpace) {
    napi_value num;

    status = napi_create_uint32(env, drive.bytesPerSect, &num);
    assert(status == napi_ok);
    status = napi_set_named_property(env, obj, "bytesPerSect", num);
    assert(status == napi_ok);

    status = napi_create_uint32(env, drive.freeClusters, &num);
    assert(status == napi_ok);
    status = napi_set_named_property(env, obj, "freeClusters", num);
    assert(status == napi_ok);

    status = napi_create_uint32(env, drive.totalClusters, &num);
    assert(status == napi_ok);
    status = napi_set_named_property(env, obj, "totalClusters", num);
    assert(status == napi_ok);

    status = napi_create_double(env, drive.usedClusterPercent, &num);
    assert(status == napi_ok);
    status = napi_set_named_property(env, obj, "usedClusterPercent", num);
    assert(status == napi_ok);

    status = napi_create_double(env, drive.freeClusterPercent, &num);
    assert(status == napi_ok);
    status = napi_set_named_property(env, obj, "freeClusterPercent", num);
    assert(status == napi_ok);
  }

  return obj;
}

static napi_value GetLogicalDrives(napi_env env, napi_callback_info info) {
  std::vector<LogicalDrive> drives;
  std::string error;

  if (!CollectLogicalDrives(&drives, &error)) {
    napi_throw_error(env, nullptr, error.c_str());
    return nullptr;
  }

  napi_status status;
  napi_value array;
  status = napi_create_array_with_length(env, drives.size(), &array);
  assert(status == napi_ok);

  for (size_t i = 0; i < drives.size(); ++i) {
    napi_value item = LogicalDriveToObject(env, drives[i]);
    status = napi_set_element(env, array, i, item);
    assert(status == napi_ok);
  }

  return array;
}
#else
static napi_value GetLogicalDrives(napi_env env, napi_callback_info info) {
  napi_throw_error(env, nullptr, "windrive is only available on Windows");
  return nullptr;
}
#endif

static napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_value fn;

  status = napi_create_function(env, "getLogicalDrives", NAPI_AUTO_LENGTH,
                                GetLogicalDrives, nullptr, &fn);
  assert(status == napi_ok);
  status = napi_set_named_property(env, exports, "getLogicalDrives", fn);
  assert(status == napi_ok);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
