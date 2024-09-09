import { ApiProperty } from '@nestjs/swagger';
import {
    IsBoolean,
    IsNotEmpty,
    Min,
    IsInt,
    Max,
    IsArray,
    IsIn,
    MaxLength,
    MinLength,
} from 'class-validator';

export class FactoryDevice {
    public getDeviceInfo(): DeviceInfo {
        return {
            hostname: 'sn-001.bionic-h2020.eu',
            kernel: '4.15.0-96-generic',
            fwVersion: 0x001001,
            swVersion: 0x000201,
            hwVersion: 0x102000,
            avgLoad: [0, 0, 0],
            freeMem: 0,
            totalMem: 0,
            freeDiskSpace: 0,
            totalDiskSpace: 0,
            upTime: 0,
        };
    }
}

export class DeviceInfo {
    @ApiProperty({
        description: 'hostname of ai chip',
    })
    hostname: string;

    @ApiProperty({
        description: 'kernel of ai chip',
    })
    kernel: string;

    @ApiProperty({
        description: 'version number of the hardware',
    })
    hwVersion: number;

    @ApiProperty({
        description: 'version number of the software',
    })
    swVersion: number;

    @ApiProperty({
        description: 'version number of the firmware',
    })
    fwVersion: number;

    @ApiProperty({
        description: 'free memory on the ai chip',
    })
    freeMem: number;

    @ApiProperty({
        description: 'total memory on the ai chip',
    })
    totalMem: number;

    @ApiProperty({
        description: 'free diskspace on the ai chip',
    })
    freeDiskSpace: number;

    @ApiProperty({
        description: 'total diskspace on the ai chip',
    })
    totalDiskSpace: number;

    @ApiProperty({
        description: 'uptime of the ai chip ',
    })
    upTime: number;

    @ApiProperty({
        description: 'average load of the system',
        type: [Number],
    })
    avgLoad: number[];
}

export class DeviceSettings {
    @IsNotEmpty()
    @IsBoolean()
    @ApiProperty({
        description: 'enable or disables notification on the smartwatch',
        type: Boolean,
        default: true,
    })
    notificationOnSmartWatch: boolean;

    @IsNotEmpty()
    @IsBoolean()
    @ApiProperty({
        description: 'enable or disable the vibration for a notification',
        type: Boolean,
        default: true,
    })
    notificationWithVibration: boolean;

    @IsArray()
    @IsInt({
        each: true,
    })
    @Max(256, {
        each: true,
    })
    @Min(0, {
        each: true,
    })
    @IsNotEmpty()
    @ApiProperty({
        description: 'the mac of the connected smartwatch',
        type: [Number],
    })
    connectedSmartWatchMAC: Array<number>;
}
