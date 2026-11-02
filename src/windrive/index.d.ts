export type DriveType =
  | 'UNKNOWN'
  | 'NO_ROOT_DIR'
  | 'REMOVABLE'
  | 'FIXED'
  | 'REMOTE'
  | 'CDROM'
  | 'RAMDISK'

export interface LogicalDrive {
  /** Drive root path, e.g. `C:\\` */
  name: string
  type: DriveType
  /** Present when free-space info is available (not for CDROM / failed queries). */
  bytesPerSect?: number
  freeClusters?: number
  totalClusters?: number
  usedClusterPercent?: number
  freeClusterPercent?: number
}

/** Enumerate Windows logical drives with type and optional space info. */
export function getLogicalDrives(): LogicalDrive[]

/** Return drive root paths only, e.g. `['C:\\', 'D:\\']`. */
export function getDriveLetters(): string[]
