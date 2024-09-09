import {
    Controller,
    Get,
    Body,
    Post,
    UsePipes,
    ValidationPipe,
    UseInterceptors,
    UploadedFile
} from '@nestjs/common';

import { Express } from 'express';
import { FileInterceptor } from '@nestjs/platform-express';
import { DeviceInfo, DeviceSettings } from './device.dto';
import { ApiOkResponse, ApiTags } from '@nestjs/swagger';
import { DeviceService } from './device.service';
import { diskStorage } from 'multer';

@ApiTags('device')
@Controller('device')
export class DeviceController {
    public constructor(private deviceService: DeviceService) {}

    @Get('info')
    @ApiOkResponse({
        description: 'The current Info information object.',
        type: DeviceInfo,
    })
    public getInfo(): DeviceInfo {
        return this.deviceService.getInfo();
    }

    @Get('settings')
    @ApiOkResponse({
        description: 'The current Settings information object.',
        type: DeviceSettings,
    })
    public getSettings(): DeviceSettings {
        return this.deviceService.getSettings();
    }

    @Post('settings')
    @UsePipes(
        new ValidationPipe({
            validationError: { target: false, value: false },
        }),
    )
    @ApiOkResponse({
        description: 'update the current Settings wit the new settings',
        type: DeviceSettings,
    })
    public setSettings(@Body() newSettings: DeviceSettings): DeviceSettings {
        return this.deviceService.setSettings(newSettings);
    }

    @Post( 'update' )
    @ApiOkResponse({
        description: 'update the existing firmware with the received .deb package.'
    })
    @UseInterceptors(
        FileInterceptor( 'file', {
            storage : diskStorage({
                    destination : './deb_packages',
                    filename    : ( req, file, callback ) => {
                        callback( null, file.originalname);
                    }
                })
        })
    )
    uploadFile( @UploadedFile() file: Express.Multer.File) 
    {
        return this.deviceService.firmwareUpdateStatus( file );
    }
}
